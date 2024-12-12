#include "kmp.h"

namespace search::engine {

void Kmp::UsePattern(std::string const &pattern) {
    this->pattern_ = pattern;
    this->PreprocessLSP();
}

std::vector<std::size_t> Kmp::SearchIn(const std::string &text) {
    std::vector<std::size_t> indecies;

    int n = text.length();
    int m = this->pattern_.length();

    // Text index.
    int i = 0;

    // Pattern index.
    int j = 0;

    while (i < n) {
        if (this->pattern_[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            indecies.push_back(i - j);
            j = this->lps_[j - 1];
        } else if (i < n && this->pattern_[j] != text[i]) {
            if (j != 0) {
                j = this->lps_[j - 1];
            } else {
                i++;
            }
        }
    }

    return indecies;
}

void Kmp::PreprocessLSP() {
    int m = this->pattern_.length();

    this->lps_ = std::vector<int>(m, 0);

    // Previous matched prefix
    int length = 0;

    int i = 1;

    while (i < m) {
        if (this->pattern_[i] == this->pattern_[length]) {
            length++;
            this->lps_[i] = length;
            i++;
        } else {
            if (length != 0) {
                // Return to previous prefix position.
                length = this->lps_[length - 1];
            } else {
                // No matches.
                this->lps_[i] = 0;
                i++;
            }
        }
    }
}

} // namespace search::engine
