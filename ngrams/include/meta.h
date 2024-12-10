#ifndef NGRAMS_META_H
#define NGRAMS_META_H

#include "entry.h"

#include <string>
#include <vector>

namespace ngrams {

struct Meta {
  inline bool Found() const { return entries.size() > 0; }
 
  // Name of the source, where target have been searched.
  std::string source_name;

  // Number of entries in the source.
  std::vector<Entry> entries;
};

} // namespace ngrams

#endif // NGRAMS_META_H