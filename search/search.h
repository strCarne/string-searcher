#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <thread>
#include <vector>

namespace search {
struct Entry {
    int line_number;
    int line_offset;
    std::string line_content;
};

struct Result {
    std::string file_path;
    std::vector<Entry> entries;
};

struct Options {
    bool case_sensitive = true;
};

std::vector<Result> DoMultithreaded(
    std::string const &root_path, std::string const &target,
    std::size_t threads_count = std::thread::hardware_concurrency());

std::vector<Result> Do(std::string const &root_path, std::string const &target);

} // namespace search


#endif // SEARCH_H
