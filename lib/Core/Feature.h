
#ifndef KLEE_FEATURE_H
#define KLEE_FEATURE_H

#include "klee/ExecutionState.h"

#include <map>
#include <vector>

using namespace klee;

namespace klee {
  class Executor;

class Feature {
public:
  static double criterion;
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked) = 0;
};

class SmallestInstructionsStepped : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestInstructionsSinceCovNew : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestCallPathInstructions : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class ClosestToUncoveredInstruction : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestAddressSpace : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LargestAddressSpace : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestSymbolics : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LargestSymbolics : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class HighestNumOfConstExpr : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class HighestNumOfSymExpr : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestNumOfSymbolicBranches : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class HighestNumOfSymbolicBranches : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LowestQueryCost : public Feature {
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

} // End klee namespace

#endif /* KLEE_FEATURE_H */