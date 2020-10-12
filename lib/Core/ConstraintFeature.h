
#ifndef KLEE_CONSTRAINTFEATURE_H
#define KLEE_CONSTRAINTFEATURE_H

#include "Feature.h"

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

class LowestQueryCost : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class HighestQueryCost : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class ShallowestState : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class DeepestState : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class ShortestConstraints : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LongestConstraints : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

} // End klee namespace

#endif /* KLEE_CONSTRAINTFEATURE_H */