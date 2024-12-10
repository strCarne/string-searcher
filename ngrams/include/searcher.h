#ifndef NGRAMS_SEARCHER_H
#define NGRAMS_SEARCHER_H

#include "meta.h"
#include "source.h"

#include <memory>
#include <string>
#include <vector>

namespace ngrams {

class Searcher {
public:
  template <typename T> using Unique = std::unique_ptr<T>;

  // Searches the 'source' for 'target' entries.
  virtual Meta Search(Source const &source, std::string target) = 0;

  // Searches multiple 'sources' for 'target' entries.
  // Default implementation is single threaded.
  virtual inline std::vector<Meta>
  SearchMultiple(std::vector<Unique<Source>> sources, std::string target) {
    std::vector<Meta> results;
    results.reserve(sources.size());

    for (auto const &source : sources) {
      Meta meta = this->Search(*source, target);

      results.push_back(meta);
    }

    return results;
  }

  virtual ~Searcher() {}
};

} // namespace ngrams

#endif // NGRAMS_SEARCHER_H