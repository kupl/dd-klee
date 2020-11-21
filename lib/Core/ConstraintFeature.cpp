
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

std::set<std::pair<double, ExecutionState*>>
FQueryCost::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    double qc = st->queryCost.toSeconds();
    st_set.insert(std::make_pair(qc, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FDepth::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    double depth = (double)st->depth;
    st_set.insert(std::make_pair(depth, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FConstraints::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    double constraintsSize = (double)st->constraints.size();
    st_set.insert(std::make_pair(constraintsSize, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FCallPathLoopDepth::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;
  
  llvm::DominatorTree DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> KLoop;

  for(const auto &st : states) {
    BasicBlock *bb = st->pc->inst->getParent();
    DT = llvm::DominatorTree(*(st->stack.back().kf->function));
    KLoop = llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    KLoop.releaseMemory();
    KLoop.analyze(DT);
    double CPLoopDepth = (double)KLoop.getLoopDepth(bb); 
    st_set.insert(std::make_pair(CPLoopDepth, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FCallerLoopDepth::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;
  
  llvm::DominatorTree DT;
  llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop> KLoop;

  for(const auto &st : states) {
    KInstIterator caller = st->stack.back().caller;
    double CSLoopDepth;
    if (caller) {
      BasicBlock *bb = st->stack.back().caller->inst->getParent();
      DT = llvm::DominatorTree(*(caller->inst->getFunction()));
      KLoop = llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
      KLoop.releaseMemory();
      KLoop.analyze(DT);
      CSLoopDepth = (double)KLoop.getLoopDepth(bb);
    } else { // main function
      CSLoopDepth = 0.0;
    }
    st_set.insert(std::make_pair(CSLoopDepth, st));
  }

  return st_set;
}