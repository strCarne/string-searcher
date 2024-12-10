#ifndef NGRAMS_SOURCE_H
#define NGRAMS_SOURCE_H

#include <string>

namespace ngrams {

// Interface that provies access to its data.
class Source {
public:
  // GetName returns name of the source. It may be file name, or something else
  // depending on the source.
  virtual std::string GetName() const = 0;

  // ReadN reads n characters from the source.
  // If source is exhausted, it returns string which length is less than n.
  virtual std::string ReadN(int n) const = 0;

  virtual ~Source() {}
};

} // namespace ngrams

#endif // NGRAMS_SOURCE_H