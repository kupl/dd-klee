
#ifndef KLEE_FEATURE_H
#define KLEE_FEATURE_H

#include "klee/ExecutionState.h"

#include <map>
#include <vector>

using namespace klee;

namespace klee {
  class Executor;

class Feature {
protected:
  static double criterion;
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states) = 0;
};

class SmallestInstructionsStepped : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestInstructionsSinceCovNew : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestCallPathInstructions : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class ClosestToUncoveredInstruction : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestAddressSpace : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class LargestAddressSpace : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestSymbolics : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class LargestSymbolics : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class HighestNumOfConstExpr : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class HighestNumOfSymExpr : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestNumOfSymbolicBranches : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class HighestNumOfSymbolicBranches : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class LowestQueryCost : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class ShallowestState : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class DeepestState : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class ShortestConstraints : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

} // End klee namespace

#endif /* KLEE_FEATURE_H */