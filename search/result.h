#pragma once

#include <string>
#include <vector>

#include "entry.h"

namespace search {

struct Result {
    std::string file_path;
    std::vector<Entry> entries;
};

} // namespace search
