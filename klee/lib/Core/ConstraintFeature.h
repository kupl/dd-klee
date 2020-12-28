
#ifndef KLEE_CONSTRAINTFEATURE_H
#define KLEE_CONSTRAINTFEATURE_H

#include "Feature.h"

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

class FQueryCost : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FDepth : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FConstraints : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FCallPathLoopDepth : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FCallerLoopDepth : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

} // End klee namespace

#endif /* KLEE_CONSTRAINTFEATURE_H */