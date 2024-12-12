#pragma once

#include <cstddef>
#include <string>

namespace search {

struct Entry {
    std::size_t line_number;
    std::size_t line_offset;
    std::string line_content;
};

} // namespace search
