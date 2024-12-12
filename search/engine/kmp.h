#pragma once

#include "engine.h"

namespace search::engine {

// KMP - Knuth-Morris-Pratt
class Kmp : public Engine {
public:
    explicit inline Kmp(const std::string &pattern) {
        this->UsePattern(pattern);
    }

    void UsePattern(std::string const &pattern) override;

    std::vector<std::size_t> SearchIn(const std::string &text) override;

private:
    std::vector<int> lps_;

    void PreprocessLSP();
};

} // namespace search::engine
