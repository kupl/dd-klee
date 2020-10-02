
#include "CallPathManager.h"
#include "CoreStats.h"
#include "Executor.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "StatsTracker.h"

#include "klee/ExecutionState.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"

#include <set>
#include <vector>

using namespace klee;
using namespace llvm;

double Feature::criterion = 0.1;

template <typename T, typename Pred = std::less<std::pair<T, std::pair<ExecutionState*, size_t>>>>
inline std::vector<bool> markFeature(
    const std::set<std::pair<T, std::pair<ExecutionState*, size_t>>, Pred> &st_set,
    std::vector<bool> &marked) {

  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * Feature::criterion);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    marked[(it->second).second] = true;
  }

  return marked;
}

// Features related to instruction history
std::vector<bool> SmallestInstructionsStepped::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (steppedInstructions, (ExecutionState*, index of state)) sorted by steppedInstructions
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t steppedInstructions = st->steppedInstructions;
    st_set.insert(std::make_pair(steppedInstructions, std::make_pair(st, i++)));
  }

  return markFeature<uint64_t>(st_set, marked);
}

std::vector<bool> SmallestInstructionsSinceCovNew::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (instsSinceCovNew, (ExecutionState*, index of state)) sorted by instsSinceCovNew
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int instsSinceCovNew = st->instsSinceCovNew;
    st_set.insert(std::make_pair(instsSinceCovNew, std::make_pair(st, i++)));
  }

  return markFeature<unsigned int>(st_set, marked);
}

std::vector<bool> SmallestCallPathInstructions::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (CallPathInstructions, (ExecutionState*, index of state)) sorted by CallPathInstructions
  // CallPathInstruction: instructions in currently executing function
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t CPInsts = st->stack.back().callPathNode->statistics.getValue(stats::instructions);
    st_set.insert(std::make_pair(CPInsts, std::make_pair(st, i++)));
  }

  return markFeature<uint64_t>(st_set, marked);
}

std::vector<bool> ClosestToUncoveredInstruction::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (md2u, (ExecutionState*, index of state)) sorted by md2u
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    StackFrame &sf = st->stack.back();
    uint64_t md2u = computeMinDistToUncovered(st->pc, sf.minDistToUncoveredOnReturn);
    st_set.insert(std::make_pair(md2u, std::make_pair(st, i++)));
  }

  return markFeature<uint64_t>(st_set, marked);
}

// Features related to symbolic memory state
std::vector<bool> SmallestAddressSpace::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (addressSpaceSize, (ExecutionState*, index of state)) stored by addressSpaceSize
  // with ascending order
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;
  
  size_t i = 0;
  for(const auto &st: states) {
    size_t addressSpaceSize = st->addressSpace.objects.size();
    st_set.insert(std::make_pair(addressSpaceSize, std::make_pair(st, i++)));
  }

  return markFeature<size_t>(st_set, marked);
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

  size_t i = 0;
  for(const auto &st: states) {
    size_t symSize = st->symbolics.size();
    st_set.insert(std::make_pair(symSize, std::make_pair(st, i++)));
  }

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

// Features related to path condition
std::vector<bool> SmallestNumOfSymbolicBranches::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (symBrCnt, (ExecutionState*, index of state)) sorted by symBrCnt with ascending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    unsigned int symBrCnt = st->symBrCount;
    st_set.insert(std::make_pair(symBrCnt, std::make_pair(st, i++)));
  }

  return markFeature<unsigned int>(st_set, marked);
}

std::vector<bool> HighestNumOfSymbolicBranches::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (symBrCnt, (ExecutionState*, index of state)) sorted by symBrCnt with descending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    unsigned int symBrCnt = st->symBrCount;
    st_set.insert(std::make_pair(symBrCnt, std::make_pair(st, i++)));
  }

  return markFeature<unsigned int, std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> LowestQueryCost::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (queryCost, (ExecutionState*, index of state)) sorted by queryCost
  std::set<std::pair<double, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    double qc = st->queryCost.toSeconds();
    st_set.insert(std::make_pair(qc, std::make_pair(st, i++)));
  }

  return markFeature<double>(st_set, marked);
}

std::vector<bool> ShallowestState::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (depth, (ExecutionState*, index of state)) sorted by depth with ascending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int depth = st->depth;
    st_set.insert(std::make_pair(depth, std::make_pair(st, i++)));
  }

  return markFeature<unsigned int>(st_set, marked);
}

std::vector<bool> DeepestState::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (depth, (ExecutionState*, index of state)) sorted by depth with descending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int depth = st->depth;
    st_set.insert(std::make_pair(depth, std::make_pair(st, i++)));
  }

  return markFeature<unsigned int, std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> ShortestConstraints::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (constraintsSize, (ExecutionState*, index of state)) sorted by constraintsSize
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    size_t constraintsSize = st->constraints.size();
    st_set.insert(std::make_pair(constraintsSize, std::make_pair(st, i++)));
  }

  return markFeature<size_t>(st_set, marked);
}
