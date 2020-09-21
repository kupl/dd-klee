
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

class NextInstExternalFunctionCall : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstFPOperation : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstAggregateOperation : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstVectorOperation : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstSwitchWithSym : public Feature {
  Executor &executor;
public:
  NextInstSwitchWithSym(Executor &_executor);
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstAllocaWithSym : public Feature {
  Executor &executor;
public:
  NextInstAllocaWithSym(Executor &_executor);
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstStoreWithSym : public Feature {
  Executor &executor;
public:
  NextInstStoreWithSym(Executor &_executor);
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NextInstIndirectBrWithSym : public Feature {
  // NOTE: const reference type cannot call executor.toUnique
  Executor &executor;
public:
  NextInstIndirectBrWithSym(Executor &_executor);
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestInstructionStepped : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestInstructionsSinceCovNew : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class SmallestCallPathInstruction : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class ClosestToUncoveredInst : public Feature {
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