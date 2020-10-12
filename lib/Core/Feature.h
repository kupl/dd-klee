
#ifndef KLEE_FEATURE_H
#define KLEE_FEATURE_H

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

class Feature {
public:
  static double criterion;
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked) = 0;
};

template <typename T, typename Pred = std::less<std::pair<T, std::pair<ExecutionState*, size_t>>>>
inline std::vector<bool> markFeature(
    const std::set<std::pair<T, std::pair<ExecutionState*, size_t>>, Pred> &st_set,
    std::vector<bool> &marked) {

  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * Feature::criterion);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    marked[(it->second).second] = true;
  }

  return marked;
}

} // End klee namespace

#endif /* KLEE_FEATURE_H */