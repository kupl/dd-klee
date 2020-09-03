
#ifndef KLEE_FEATURE_H
#define KLEE_FEATURE_H

#include "klee/ExecutionState.h"

#include <map>
#include <vector>

using namespace klee;

class Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states) = 0;
};

class NXTInstExternalFunctionCall : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NXTInstFPOperation : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NXTInstAggregateOperation : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

class NXTInstVectorOperation : public Feature {
public:
  virtual std::vector<bool> operator()(const std::vector<ExecutionState*> &states);
};

#endif /* KLEE_FEATURE_H */
