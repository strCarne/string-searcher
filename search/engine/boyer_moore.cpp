#include "boyer_moore.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace search::engine {

void BoyerMoore::UsePattern(std::string const &pattern) {
    this->pattern_ = pattern;

    int m = pattern.length();
    for (int i = 0; i < m; ++i) {
        this->bad_char_heuristic_[pattern[i]] = m - i - 1;
    }
}

std::vector<std::size_t> BoyerMoore::SearchIn(const std::string &text) {
    int n = text.length();
    int m = this->pattern_.length();
    std::vector<std::size_t> result;

    if (m == 0 || n < m)
        return result;

    int i = 0; // Позиция в тексте
    while (i <= n - m) {
        int j = m - 1;

        // Сравниваем паттерн с подстрокой в тексте
        while (j >= 0 && this->pattern_[j] == text[i + j]) {
            --j;
        }

        // Если паттерн найден
        if (j < 0) {
            result.push_back(i);
            i += (i + m < n) ? m - this->bad_char_heuristic_[text[i + m]] : 1;
        } else {
            // Иначе сдвигаем на количество, соответствующее плохому символу
            i += std::max(1, this->bad_char_heuristic_[text[i + j]]);
        }
    }

    return result;
}

} // namespace search::engine
