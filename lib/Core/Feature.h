
#ifndef KLEE_FEATURE_H
#define KLEE_FEATURE_H

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

class Feature {
public:
  virtual std::vector<double>
  operator()(const std::vector<ExecutionState*> &states) = 0;
};

template <typename T>
inline std::vector<double>
normalizeFeature(const std::set<std::pair<T, std::pair<ExecutionState*, size_t>>> &st_set) {
  T max_value = st_set.rbegin()->first;
  T min_value = st_set.begin()->first;
  std::vector<double> fvalues(st_set.size(), 0.0);

  // min-max normalization for feature values
  if (max_value != min_value) {
    for(auto it = st_set.cbegin(); it != st_set.cend(); it++) {
      fvalues[(it->second).second] = 
        (double) (it->first - min_value) / (max_value - min_value);
    }
  }

  return fvalues;
}

} // End klee namespace

#endif /* KLEE_FEATURE_H */