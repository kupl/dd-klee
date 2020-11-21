
#ifndef KLEE_FEATURE_H
#define KLEE_FEATURE_H

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

class Feature {
public:
  static double criterion;

  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states) = 0;
};

} // End klee namespace

#endif /* KLEE_FEATURE_H */