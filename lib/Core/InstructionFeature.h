
#ifndef KLEE_INSTRUCTIONFEATURE_H
#define KLEE_INSTRUCTIONFEATURE_H

#include "Feature.h"

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {
class FInstructionsStepped : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FInstructionsSinceCovNew : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FCallPathInstructions : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

class FMinDistToUncoveredInst : public Feature {
public:
  virtual std::set<std::pair<double, ExecutionState*>>
  operator()(const std::vector<ExecutionState*> &states);
};

} // End klee namespace

#endif /* KLEE_INSTRUCTIONFEATURE_H */