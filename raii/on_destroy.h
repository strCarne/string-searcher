#pragma once

#include <functional>

namespace raii {

template <typename F, typename... Args> class OnDestroy {
public:
    inline OnDestroy(F &&f, Args &&...args)
        : defered_call_(
              std::bind(std::forward<F>(f), std::forward<Args>(args)...)) {}

    inline ~OnDestroy() { defered_call_(); }

private:
    std::function<void()> defered_call_;
};

} // namespace raii
