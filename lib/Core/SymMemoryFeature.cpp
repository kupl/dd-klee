
#include "Executor.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "SymMemoryFeature.h"

#include "klee/ExecutionState.h"

#include <set>
#include <vector>

using namespace klee;
using namespace llvm;

std::vector<bool> SmallestAddressSpace::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (addressSpaceSize, (ExecutionState*, index of state)) stored by addressSpaceSize
  // with ascending order
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<size_t> val_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t addressSpaceSize = st->addressSpace.objects.size();
    st_set.insert(std::make_pair(addressSpaceSize, std::make_pair(st, i++)));
    val_set.insert(addressSpaceSize);
  }

  stats::uniqueRatioAddrSpace += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<size_t, std::less<std::pair<size_t, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> LargestAddressSpace::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (addressSpaceSize, (ExecutionState*, index of state)) stored by addressSpaceSize
  // with descending order
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t addressSpaceSize = st->addressSpace.objects.size();
    st_set.insert(std::make_pair(addressSpaceSize, std::make_pair(st, i++)));
  }

  return markFeature<size_t, std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> SmallestSymbolics::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (symbolics.size(), (ExecutionState*, index of state)) sorted by symbolics.size()
  // with ascending order 
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<size_t> val_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t symSize = st->symbolics.size();
    st_set.insert(std::make_pair(symSize, std::make_pair(st, i++)));
    val_set.insert(symSize);
  }

  stats::uniqueRatioSymbolics += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<size_t>(st_set, marked);
}

std::vector<bool> LargestSymbolics::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (symbolics.size(), (ExecutionState*, index of state)) sorted by symbolics.size()
  // with descending order 
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t symSize = st->symbolics.size();
    st_set.insert(std::make_pair(symSize, std::make_pair(st, i++)));
  }

  return markFeature<size_t, std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> SmallestNumOfConstExpr::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (constCnt, (ExecutionState*, index of state)) sorted by constCnt
  // with ascending order
  // constCnt: the number of local variables mapped with concrete values;
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<unsigned int> val_set;

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
    val_set.insert(constCnt);
  }

  stats::uniqueRatioConcreteExprCount += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<unsigned int, std::less<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> HighestNumOfConstExpr::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (constCnt, (ExecutionState*, index of state)) sorted by constCnt
  // with descending order
  // constCnt: the number of local variables mapped with concrete values;
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>> st_set;

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

  return markFeature<unsigned int, std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> SmallestNumOfSymExpr::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (symCnt, (ExecutionState*, index of state)) sorted by symCnt
  // with ascending order 
  // symCnt: the number of local variables mapped with symbolic expressions
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<unsigned int> val_set;

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
    val_set.insert(symCnt);
  }

  stats::uniqueRatioSymbolicExprCount += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<unsigned int, std::less<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}


std::vector<bool> HighestNumOfSymExpr::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (symCnt, (ExecutionState*, index of state)) sorted by symCnt
  // with descending order 
  // symCnt: the number of local variables mapped with symbolic expressions
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>> st_set;

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

  return markFeature<unsigned int, std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}
