#ifndef NGRAMS_ENTRY_H
#define NGRAMS_ENTRY_H

#include <cstddef>
namespace ngrams {

struct Entry {
  // Holds the global offset of the entry in the source.
  std::size_t global_offset;

  // Holds the line number starting from 0.
  std::size_t line_number;

  // Holds the offset from the beginning of the line.
  std::size_t line_offset;
};

}

#endif // NGRAMS_ENTRY_H