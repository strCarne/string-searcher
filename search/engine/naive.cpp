#include "naive.h"

namespace search::engine {
void Naive::UsePattern(std::string const &pattern) { this->pattern_ = pattern; }

std::vector<std::size_t> Naive::SearchIn(const std::string &text) {
    std::vector<std::size_t> matches;

    if (text.length() < this->pattern_.length()) {
        return matches;
    }

    for (std::size_t i = 0; i <= text.length() - this->pattern_.length(); ++i) {
        if (this->pattern_ == text.substr(i, this->pattern_.length())) {
            matches.push_back(i);
        }
    }

    return matches;
}

} // namespace search::engine
