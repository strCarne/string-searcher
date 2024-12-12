#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine.h"

namespace search::engine {

class BoyerMoore : public Engine {
public:
    explicit inline BoyerMoore(const std::string &pattern) {
        this->UsePattern(pattern);
    }

    void UsePattern(std::string const &pattern) override;

    std::vector<std::size_t> SearchIn(const std::string &text) override;

private:
    std::unordered_map<char, int> bad_char_heuristic_;
};

} // namespace search::engine
