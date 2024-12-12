#include "searcher.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "../raii/on_destroy.h"
#include "entry.h"

namespace search {

Searcher::Ok<std::unordered_map<Searcher::FileName, Searcher::GuardedEntries>>
Searcher::Search(std::string const &root_path, std::string const &target) {
    // Making sure there are no previous search results.
    this->search_results_.clear();

    // There is nothing to find.
    if (target.empty()) {
        std::cerr
            << "[search::Searcher::Search] nothing to find: target is empty\n";
        return {.ok = false};
    }

    this->search_engine_->UsePattern(target);

    // Check if root_path exists.
    struct stat stat_buf;
    if (lstat(root_path.c_str(), &stat_buf) != 0) {
        std::cerr << "[search::Searcher::Search] failed to get [" << root_path
                  << "] stat\n";

        return {.ok = false};
    }

    bool ok;
    if (stat_buf.st_mode & S_IFDIR) {
        ok = this->IterateRecursively(root_path);
    } else {
        ok = this->SearchInFile(root_path);
    }

    this->pool_.wait();

    return {.result = std::move(this->search_results_), .ok = ok};
}

bool Searcher::IterateRecursively(std::string const &path) {
    auto directory = opendir(path.c_str());
    if (directory == NULL) {
        std::cerr << "[search::Searcher::IterateRecursively] failed to open "
                  << path << ": " << strerror(errno) << '\n';

        return false;
    }

    auto defered_close_dir = raii::OnDestroy([directory, path]() {
        if (closedir(directory) != 0) {
            std::cerr << "[search::Searcher::IterateRecursively] failed to "
                         "close directory "
                      << path << ": " << strerror(errno) << '\n';
        }
    });

    while (auto entry = readdir(directory)) {
        // Skip processing of current and parent directories.
        if (std::strcmp(entry->d_name, ".") == 0 ||
            std::strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string entry_path = path + "/" + entry->d_name;

        struct stat stat_buf;
        if (lstat(entry_path.c_str(), &stat_buf) != 0) {
            std::cerr << "[search::Searcher::IterateRecursively] failed to get ["
                      << entry_path << "] stat" << ": " << strerror(errno) << '\n';

            return false;
        }

        if (S_ISDIR(stat_buf.st_mode)) { // is a directory
            if (this->IsIgnoredDirectory(entry->d_name)) {
                continue;
            }

            bool ok = this->IterateRecursively(entry_path);
            if (!ok) {
                std::cerr << "[search::Searcher::IterateRecursively] failed to "
                          << "iterate in [" << entry_path << "]" << ": "
                          << strerror(errno) << '\n';

                // return false;
            }
        } else if (S_ISREG(stat_buf.st_mode)) { // is a file
            if (this->IsIgnoredFile(entry->d_name)) {
                continue;
            }

            this->pool_.submit([this, entry_path]() {
                bool ok = this->SearchInFile(entry_path);
                if (!ok) {
                    std::cerr << "[search::Searcher::IterateRecursively] failed to "
                              << "search in [" << entry_path << "]" << ": "
                              << strerror(errno) << '\n';
                }
            });
        }
    }

    return true;
}

bool Searcher::SearchInFile(std::string const &path) {
    struct stat stat_buf;
    if (lstat(path.c_str(), &stat_buf) != 0) {
        std::cerr << "[search::Searcher::SearchInFile] " << "failed to get ["
                  << path << "] " << "stat" << ": " << strerror(errno) << '\n';

        return false;
    }

    bool ok;
    if (stat_buf.st_size < Searcher::kFileSizeSingleThreadedProcessingLimit) {
        ok = this->SingleThreadedSearchInFile(path);
    } else {
        ok = this->SingleThreadedSearchInFile(path);
        // TODO: switch to multithreaded
        // ok = this->MultiThreadedSearchInFile(path, target);
    }

    return ok;
}

bool Searcher::SingleThreadedSearchInFile(std::string const &path) {
    FILE *file = fopen(path.c_str(), "r");

    if (file == nullptr) {
        std::cerr << "[search::Searcher::SignleThreadedSearchInFile] failed to "
                  << "open [" << path << "]" << ": " << strerror(errno) << '\n';

        return false;
    }

    auto defered_close_file = raii::OnDestroy([file, &path]() {
        if (fclose(file) != 0) {
            std::cerr << "[search::Searcher::SignleThreadedSearchInFile] failed to "
                      << "close [" << path << "]" << ": " << strerror(errno) << '\n';
        }
    });

    // Used by 'getline'.
    char *line = NULL;
    std::size_t len = 0;
    ssize_t nread;
    auto defered_line_free = raii::OnDestroy([=] { free(line); });

    std::vector<Entry> local_results;

    std::size_t line_number = 0;

    while ((nread = getline(&line, &len, file)) != -1) {
        line_number += 1;

        auto result = this->search_engine_->SearchIn(line);

        for (auto line_offset : result) {
            local_results.push_back({.line_number = line_number,
                                     .line_offset = line_offset,
                                     .line_content = line + line_offset});
        }
    }

    if (!local_results.empty()) {
        GuardedEntries *entries = nullptr;

        { // Locking 'search_results_' to retrieve entries for certain path
            pthread_mutex_lock(&this->search_results_guard_);
            auto defered_search_results_guard_unlock = raii::OnDestroy(
                [this]() { pthread_mutex_unlock(&this->search_results_guard_); });

            auto has_entry =
                this->search_results_.find(path) != this->search_results_.end();
            if (!has_entry) {
                this->search_results_[path] = GuardedEntries();
            }

            entries = &this->search_results_.at(path);
        }

        { // Locking entries to add new entry
            pthread_mutex_lock(&entries->guard);
            auto defered_entries_guard_unlock = raii::OnDestroy(
                [entries]() { pthread_mutex_unlock(&entries->guard); });

            for (auto const &entry : local_results) {
                entries->entries.push_back(entry);
            }
        }
    }

    return true;
}

bool Searcher::MultiThreadedSearchInFile(std::string const &path) {
    // TODO: implement
    return false;
}

bool Searcher::IsIgnoredDirectory(std::string const &path) {
    auto ignored_dir =
        std::find(this->ignored_dirs_.begin(), this->ignored_dirs_.end(), path);

    return ignored_dir != this->ignored_dirs_.end();
}

bool Searcher::IsIgnoredFile(std::string const &path) {
    auto ignored_file =
        std::find(this->ignored_files_.begin(), this->ignored_files_.end(), path);

    return ignored_file != this->ignored_files_.end();
}

} // namespace search
