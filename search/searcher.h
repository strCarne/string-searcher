#pragma once

#include <memory>
#include <pthread.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "../thread/pool.h"
#include "engine/engine.h"
#include "entry.h"

#define MEGA_BYTES(num) (num * 1024 * 1024)
#define KILO_BYTES(num) (num * 1024)

#include "engine/kmp.h"
#define DEFAULT_ENGINE new engine::Kmp("")
#define DEFAULT_THREADS_COUNT std::thread::hardware_concurrency()

#include <cstddef>
namespace search {

class Searcher {
public:
    using FileName = std::string;
    using Entries = std::vector<Entry>;

    template <typename T> struct Ok {
        T result;
        bool ok;
    };

    struct GuardedEntries {
        Entries entries;
        pthread_mutex_t guard;

        inline GuardedEntries() { pthread_mutex_init(&this->guard, nullptr); }
    };

    inline Searcher() : Searcher(DEFAULT_ENGINE, DEFAULT_THREADS_COUNT) {}

    inline Searcher(engine::Engine *engine)
        : Searcher(engine, DEFAULT_THREADS_COUNT) {}

    inline Searcher(std::size_t threads_count)
        : Searcher(DEFAULT_ENGINE, threads_count) {}

    inline Searcher(engine::Engine *engine, std::size_t threads_count)
        : pool_(threads_count) {
        pthread_mutex_init(&this->search_results_guard_, nullptr);
        this->search_engine_.reset(DEFAULT_ENGINE);
    }

    inline std::vector<std::string> &ignored_dirs() { return ignored_dirs_; }
    inline std::vector<std::string> &ignored_files() { return ignored_files_; }

    // Search searches for 'target' string in given 'root_path'.
    //
    // 'root_path' can be a directory or a file.
    // If 'root_path' is directory, then search is recursive.
    //
    // 'target' must be a not empty string.
    // If it is empty, than empty results will be returned immediately.
    //
    // Returns hash-map where key is a file name, where entries where found, and
    // value is a vector of found entries. Result contains only non-empty entries.
    Ok<std::unordered_map<FileName, GuardedEntries>>
    Search(std::string const &root_path, std::string const &target);

private:
    // IterateRecursively searches for 'target' string in 'path' directory.
    // When iterating over direcory it encounters another directory, then new
    // search is started recursively in this directory.
    //
    // 'path' - path to directory in file system.
    //
    // Returns true if search was successful, false otherwise.
    bool IterateRecursively(std::string const &path);

    // SearchInFile searches for 'target' string in 'path' file.
    //
    // 'path' - path to file in file system.
    //
    // Returns true if search was successful, false otherwise.
    bool SearchInFile(std::string const &path);

    // SingleThreadedSearchInFile searches for 'target' string in 'path' file in a
    // single threaded manner without splitting file in chunks.
    //
    // 'path' - path to file in file system.
    //
    // Returns true if search was successful, false otherwise.
    bool SingleThreadedSearchInFile(std::string const &path);

    // SingleThreadedSearchInFile searches for 'target' string in 'path' file in a
    // single threaded manner without splitting file in chunks.
    //
    // 'path' - path to file in file system.
    //
    // Returns true if search was successful, false otherwise.
    bool MultiThreadedSearchInFile(std::string const &path);

    // IsIgnoredDirectory checks if 'path' is ignored directory.
    //
    // Returns true if 'path' is ignored directory, false otherwise.
    bool IsIgnoredDirectory(std::string const &path);

    // IsIgnoredFile checks if 'path' is ignored file.
    //
    // Returns true if 'path' is ignored file, false otherwise.
    bool IsIgnoredFile(std::string const &path);

    // Search results.
    std::unordered_map<FileName, GuardedEntries> search_results_;

    // Search results guard.
    pthread_mutex_t search_results_guard_;

    // Thread pool for search tasks.
    thread::Pool pool_;

    // Ignored directories.
    std::vector<std::string> ignored_dirs_;

    // Ignored files.
    std::vector<std::string> ignored_files_;

    std::unique_ptr<engine::Engine> search_engine_;

    // The maximum size of a file that will be processed in a single thread.
    // If limit exceeded, then file will be splitted into chunks and processed
    // separately.
    static const std::size_t kFileSizeSingleThreadedProcessingLimit =
        MEGA_BYTES(10);

    static const std::size_t kReadBufferCapacity = KILO_BYTES(64);
};

} // namespace search
