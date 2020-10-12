
#ifndef KLEE_SYMMEMORYFEATURE_H
#define KLEE_SYMMEMORYFEATURE_H

#include "Feature.h"

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

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

} // End klee namespace

#endif /* KLEE_SYMMEMORY_FEATURE_H */