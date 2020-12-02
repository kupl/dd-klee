
#include "Executor.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "SymMemoryFeature.h"

#include "klee/ExecutionState.h"

#include <set>
#include <vector>

using namespace klee;
using namespace llvm;

std::set<std::pair<double, ExecutionState*>>
FAddressSpace::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st: states) {
    double addressSpaceSize = (double)st->addressSpace.objects.size();
    st_set.insert(std::make_pair(addressSpaceSize, st));
  }

  return st_set;
}


std::set<std::pair<double, ExecutionState*>>
FSymbolics::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st: states) {
    double symSize = (double)st->symbolics.size();
    st_set.insert(std::make_pair(symSize, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FNumOfConstExpr::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    unsigned int constCnt = 0;
    for(auto &sf : st->stack) {
      unsigned int numRegisters = sf.kf->numRegisters;
      Cell *locals = sf.locals;

      for(unsigned int i = 0; i < numRegisters; i++) {
        ref<Expr> value = (locals + i) -> value;
        if(!value.get())
          continue;
        if(isa<ConstantExpr>(value))
          constCnt++;
      }
    }
    st_set.insert(std::make_pair((double)constCnt, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FNumOfSymExpr::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    unsigned int symCnt = 0;
    for(auto &sf : st->stack) {
      unsigned int numRegisters = sf.kf->numRegisters;
      Cell *locals = sf.locals;

      for(unsigned int i = 0; i < numRegisters; i++) {
        ref<Expr> value = (locals + i) -> value;
        if(!value.get())
          continue;
        if(!isa<ConstantExpr>(value))
          symCnt++;
      }
    }
    st_set.insert(std::make_pair((double)symCnt, st));
  }

  return st_set;
}
