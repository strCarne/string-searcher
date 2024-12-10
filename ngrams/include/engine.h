#ifndef NGRAMS_ENGINE_H
#define NGRAMS_ENGINE_H

#include "searcher.h"

#include <string>

namespace ngrams {

class Engine : public Searcher {
public:
  Engine();
  Engine(int n);
  Engine(int n, int threads_limit);

  Meta Search(Source const &source, std::string target) override;

  std::vector<Meta> SearchMultiple(std::vector<Unique<Source>> sources,
                                   std::string target) override;

  ~Engine();

private:
  int _n;

  const int kThreadsLimit;

  const int kReadN = 10 * _n;
};

} // namespace ngrams

#endif // NGRAMS_ENGINE_H