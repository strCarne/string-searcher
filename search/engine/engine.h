#pragma once

#include <cstddef>
#include <string>
#include <vector>
namespace search::engine {

class Engine {
public:
    virtual std::vector<std::size_t> SearchIn(std::string const &text) = 0;
    virtual void UsePattern(std::string const &pattern) = 0;
    virtual std::string GetPattern() const { return this->pattern_; }

    virtual ~Engine() {}

protected:
    std::string pattern_;
};

} // namespace search::engine
