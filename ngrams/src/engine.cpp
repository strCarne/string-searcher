#include "engine.h"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace ngrams {

Meta Engine::Search(Source const &source, std::string target) {
  Meta meta;
  meta.source_name = source.GetName();

  // std::unordered_map<std::string, std::size_t>;

  std::string buffer = source.ReadN(this->kReadN);
  while (buffer.size() > 0) {
  }
}

} // namespace ngrams