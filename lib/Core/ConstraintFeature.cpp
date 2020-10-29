
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

std::vector<double>
FQueryCost::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    double qc = st->queryCost.toSeconds();
    st_set.insert(std::make_pair(qc, std::make_pair(st, i++)));
  }

  return normalizeFeature<double>(st_set);
}

std::vector<double>
FDepth::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int depth = st->depth;
    st_set.insert(std::make_pair(depth, std::make_pair(st, i++)));
  }

  return normalizeFeature<unsigned int>(st_set);
}

std::vector<double>
FConstraints::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    size_t constraintsSize = st->constraints.size();
    st_set.insert(std::make_pair(constraintsSize, std::make_pair(st, i++)));
  }

  return normalizeFeature<size_t>(st_set);
}

std::vector<double>
FCallPathLoopDepth::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<unsigned, std::pair<ExecutionState*, size_t>>> st_set;
  
  llvm::DominatorTree DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> KLoop;

  size_t i = 0;
  for(const auto &st : states) {
    BasicBlock *bb = st->pc->inst->getParent();
    DT = llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop.releaseMemory();
    KLoop.analyze(DT);
    unsigned CPLoopDepth = KLoop.getLoopDepth(bb); 
    st_set.insert(std::make_pair(CPLoopDepth, std::make_pair(st, i++)));
  }

  return normalizeFeature<unsigned>(st_set);
}

std::vector<double>
FCallerLoopDepth::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<unsigned, std::pair<ExecutionState*, size_t>>> st_set;
  
  llvm::DominatorTree DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> KLoop;

  size_t i = 0;
  for(const auto &st : states) {
    BasicBlock *bb = st->stack.back().caller->inst->getParent();
    DT = llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop.releaseMemory();
    KLoop.analyze(DT);
    unsigned CSLoopDepth = KLoop.getLoopDepth(bb); 
    st_set.insert(std::make_pair(CSLoopDepth, std::make_pair(st, i++)));
  }

  return normalizeFeature<unsigned>(st_set);
}