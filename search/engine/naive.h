#pragma once

#include <string>
#include <vector>

#include "engine.h"

namespace search::engine {

class Naive : public Engine {
public:
    explicit inline Naive(const std::string &pattern) {
        this->UsePattern(pattern);
    }

    void UsePattern(std::string const &pattern) override;

    std::vector<std::size_t> SearchIn(const std::string &text) override;
};

} // namespace search::engine
