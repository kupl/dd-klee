
#include "Executor.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "SymMemoryFeature.h"

#include "klee/ExecutionState.h"

#include <set>
#include <vector>

using namespace klee;
using namespace llvm;

std::vector<double>
FAddressSpace::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t addressSpaceSize = st->addressSpace.objects.size();
    st_set.insert(std::make_pair(addressSpaceSize, std::make_pair(st, i++)));
  }

  return normalizeFeature<size_t>(st_set);
}


std::vector<double>
FSymbolics::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t symSize = st->symbolics.size();
    st_set.insert(std::make_pair(symSize, std::make_pair(st, i++)));
  }

  return normalizeFeature<size_t>(st_set);
}

std::vector<double>
FNumOfConstExpr::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
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
    st_set.insert(std::make_pair(constCnt, std::make_pair(st, i++)));
  }

  return normalizeFeature<unsigned int>(st_set);
}

std::vector<double>
FNumOfSymExpr::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
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
    st_set.insert(std::make_pair(symCnt, std::make_pair(st, i++)));
  }

  return normalizeFeature<unsigned int>(st_set);
}
