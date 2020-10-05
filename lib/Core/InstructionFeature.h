
#ifndef KLEE_INSTRUCTIONFEATURE_H
#define KLEE_INSTRUCTIONFEATURE_H

#include "Feature.h"

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {
class SmallestInstructionsStepped : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LargestInstructionsStepped : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestInstructionsSinceCovNew : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LargestInstructionsSinceCovNew : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class SmallestCallPathInstructions : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class LargestCallPathInstructions : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class ClosestToUncoveredInstruction : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

class FarthestToUncoveredInstruction : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states,
                                       std::vector<bool> &marked);
};

} // End klee namespace

#endif /* KLEE_INSTRUCTIONFEATURE_H */