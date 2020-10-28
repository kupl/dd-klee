
#include "ConstraintFeature.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "StatsTracker.h"

#include "klee/ExecutionState.h"
#include "klee/Internal/Module/KModule.h"
#include "klee/Internal/Module/KInstruction.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"

#include <set>
#include <vector>

using namespace klee;
using namespace llvm;

std::vector<bool> LowestQueryCost::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (queryCost, (ExecutionState*, index of state)) sorted by queryCost
  // with ascending order
  std::set<std::pair<double, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<double> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    double qc = st->queryCost.toSeconds();
    st_set.insert(std::make_pair(qc, std::make_pair(st, i++)));
    val_set.insert(qc);
  }

  stats::uniqueRatioQC += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<double>(st_set, marked);
}

std::vector<bool> HighestQueryCost::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (queryCost, (ExecutionState*, index of state)) sorted by queryCost
  // with descending order
  std::set<std::pair<double, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<double, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    double qc = st->queryCost.toSeconds();
    st_set.insert(std::make_pair(qc, std::make_pair(st, i++)));
  }

  return markFeature<double, std::greater<std::pair<double, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> ShallowestState::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (depth, (ExecutionState*, index of state)) sorted by depth with ascending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<unsigned int> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int depth = st->depth;
    st_set.insert(std::make_pair(depth, std::make_pair(st, i++)));
    val_set.insert(depth);
  }

  stats::uniqueRatioDepth += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

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
  // with ascending order
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<size_t> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    size_t constraintsSize = st->constraints.size();
    st_set.insert(std::make_pair(constraintsSize, std::make_pair(st, i++)));
    val_set.insert(constraintsSize);
  }

  stats::uniqueRatioConstraints += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<size_t>(st_set, marked);
}

std::vector<bool> LongestConstraints::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (constraintsSize, (ExecutionState*, index of state)) sorted by constraintsSize
  // with descending order
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    size_t constraintsSize = st->constraints.size();
    st_set.insert(std::make_pair(constraintsSize, std::make_pair(st, i++)));
  }

  return markFeature<size_t, std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> ShallowestCPLoop::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (CPLoopDepth, (ExecutionStae*, index of state)) sorted by CPLoopDepth
  // with ascending order
  std::set<std::pair<unsigned, std::pair<ExecutionState*, size_t>>> st_set;
  
  llvm::DominatorTree *DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> *KLoop = 0;

  size_t i = 0;
  for(const auto &st : states) {
    BasicBlock *bb = st->pc->inst->getParent();
    DT = new llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = new llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop->releaseMemory();
    KLoop->analyze(*DT);
    unsigned CPLoopDepth = KLoop->getLoopDepth(bb); 
    st_set.insert(std::make_pair(CPLoopDepth, std::make_pair(st, i++)));
  }

  return markFeature<unsigned>(st_set, marked);
}

std::vector<bool> DeepestCPLoop::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (CPLoopDepth, (ExecutionStae*, index of state)) sorted by CPLoopDepth
  // with descending order
  std::set<std::pair<unsigned, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned, std::pair<ExecutionState*, size_t>>>> st_set;
  
  llvm::DominatorTree *DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> *KLoop = 0;

  size_t i = 0;
  for(const auto &st : states) {
    BasicBlock *bb = st->pc->inst->getParent();
    DT = new llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = new llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop->releaseMemory();
    KLoop->analyze(*DT);
    unsigned CPLoopDepth = KLoop->getLoopDepth(bb); 
    st_set.insert(std::make_pair(CPLoopDepth, std::make_pair(st, i++)));
  }

  return markFeature<unsigned, std::greater<std::pair<unsigned, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}

std::vector<bool> ShallowestCSLoop::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (CSLoopDepth, (ExecutionStae*, index of state)) sorted by CSLoopDepth
  // with ascending order
  std::set<std::pair<unsigned, std::pair<ExecutionState*, size_t>>> st_set;
  
  llvm::DominatorTree *DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> *KLoop = 0;

  size_t i = 0;
  for(const auto &st : states) {
    BasicBlock *bb = st->stack.back().caller->inst->getParent();
    DT = new llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = new llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop->releaseMemory();
    KLoop->analyze(*DT);
    unsigned CSLoopDepth = KLoop->getLoopDepth(bb); 
    st_set.insert(std::make_pair(CSLoopDepth, std::make_pair(st, i++)));
  }

  return markFeature<unsigned>(st_set, marked);
}

std::vector<bool> DeepestCSLoop::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (CSLoopDepth, (ExecutionStae*, index of state)) sorted by CSLoopDepth
  // with descending order
  std::set<std::pair<unsigned, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned, std::pair<ExecutionState*, size_t>>>> st_set;
  
  llvm::DominatorTree *DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> *KLoop = 0;

  size_t i = 0;
  for(const auto &st : states) {
    BasicBlock *bb = st->stack.back().caller->inst->getParent();
    DT = new llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = new llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop->releaseMemory();
    KLoop->analyze(*DT);
    unsigned CSLoopDepth = KLoop->getLoopDepth(bb); 
    st_set.insert(std::make_pair(CSLoopDepth, std::make_pair(st, i++)));
  }

  return markFeature<unsigned, std::greater<std::pair<unsigned, std::pair<ExecutionState*, size_t>>>>(st_set, marked);
}