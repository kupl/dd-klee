
#ifndef KLEE_SYMMEMORYFEATURE_H
#define KLEE_SYMMEMORYFEATURE_H

#include "Feature.h"

#include "klee/ExecutionState.h"

#include <vector>

using namespace klee;

namespace klee {

class FAddressSpace : public Feature {
public:
  virtual std::vector<double>
  operator()(const std::vector<ExecutionState*> &states);
};

class FSymbolics : public Feature {
public:
  virtual std::vector<double>
  operator()(const std::vector<ExecutionState*> &states);
};

class FNumOfConstExpr : public Feature {
public:
  virtual std::vector<double>
  operator()(const std::vector<ExecutionState*> &states);
};

class FNumOfSymExpr : public Feature {
public:
  virtual std::vector<double>
  operator()(const std::vector<ExecutionState*> &states);
};

} // End klee namespace

#endif /* KLEE_SYMMEMORY_FEATURE_H */