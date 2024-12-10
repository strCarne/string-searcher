#include "search.h"

#include <cstddef>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <sys/file.h>
#include <sys/stat.h>

#include "../rai/on_destroy.h"
#include "../thread/pool.h"

namespace search {

namespace {

bool CStringsAreEqual(char const *a, char const *b, size_t size) {
    for (int i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

Result SearchInFile(std::string path, std::string target) {
    auto result = Result{.file_path = path};

    FILE *file = fopen(path.c_str(), "r");

    if (file == NULL) {
        std::cerr << "failed to open [" << path << "] while SearchInFile call"
                  << '\n';
        return result;
    }
    auto defered_file_close = rai::OnDestroy([=] { fclose(file); });

    int line_number = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    auto defered_line_free = rai::OnDestroy([=] { free(line); });

    while ((nread = getline(&line, &len, file)) != -1) {
        line_number += 1;

        if (nread < target.size()) {
            continue;
        }

        for (int i = 0; i <= nread - target.size(); i++) {
            if (CStringsAreEqual(line + i, target.c_str(), target.size())) {
                result.entries.push_back({.line_number = line_number,
                                          .line_offset = i,
                                          .line_content = line + i});
            }
        }
    }

    return result;
}

// Returns false if failed
bool IterateRecursivelyMultithreaded(
    std::string const &path, std::string const &target, thread::Pool &pool,
    std::vector<std::future<search::Result>> &futures) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        std::cerr << "failed to get [" << path << "] stat\n";
        return false;
    }

    if (statbuf.st_mode & S_IFDIR) {
        auto directory = opendir(path.c_str());
        if (directory == NULL) {
            return false;
        }

        while (auto entry = readdir(directory)) {
            // Skip processing of current and parent directories.
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::string entry_path = path + "/" + entry->d_name;

            IterateRecursivelyMultithreaded(entry_path, target, pool, futures);
        }
    } else {
        auto future =
            pool.submit([path, target]() { return SearchInFile(path, target); });
        futures.push_back(std::move(future));
    }

    return true;
}

bool IterateRecursively(std::string const &path, std::string const &target,
                        std::vector<Result> &results) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        std::cerr << "failed to get [" << path << "] stat\n";
        return false;
    }

    if (statbuf.st_mode & S_IFDIR) {
        auto directory = opendir(path.c_str());
        if (directory == NULL) {
            return false;
        }

        while (auto entry = readdir(directory)) {
            // Skip processing of current and parent directories.
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::string entry_path = path + "/" + entry->d_name;

            IterateRecursively(entry_path, target, results);
        }
    } else {
        results.push_back(SearchInFile(path, target));
    }

    return true;
}

} // namespace

std::vector<search::Result> DoMultithreaded(std::string const &root_path,
                                            std::string const &target,
                                            std::size_t threads_count) {
    auto pool = thread::Pool(threads_count);
    std::vector<std::future<search::Result>> futures;

    IterateRecursivelyMultithreaded(root_path, target, pool, futures);

    std::vector<search::Result> results;
    for (auto &&future : futures) {
        results.push_back(future.get());
    }

    return results;
}

std::vector<Result> Do(std::string const &root_path,
                       std::string const &target) {
    std::vector<search::Result> results;
    IterateRecursively(root_path, target, results);
    return results;
}

} // namespace search
