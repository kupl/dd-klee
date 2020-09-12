
#include "CallPathManager.h"
#include "CoreStats.h"
#include "Executor.h"
#include "Feature.h"

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

// PARAM_TODO: categorize features and modularize calculation

// Features related to next instruction
std::vector<bool> NXTInstExternalFunctionCall::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;
  
  for(const auto &st : states) {
    Instruction *nextInst = st->pc->inst;
    unsigned int i = nextInst->getOpcode();
    
    checked.push_back((i == Instruction::Invoke) || (i == Instruction::Call));
  }

  return checked;
}

std::vector<bool> NXTInstFPOperation::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(const auto &st : states) {
    Instruction *nextInst = st->pc->inst;
    unsigned int i = nextInst->getOpcode();

    checked.push_back(i == Instruction::FAdd ||
                      i == Instruction::FSub ||
                      i == Instruction::FMul ||
                      i == Instruction::FDiv ||
                      i == Instruction::FRem ||
                      i == Instruction::FPTrunc ||
                      i == Instruction::FPExt ||
                      i == Instruction::FPToUI ||
                      i == Instruction::FPToSI ||
                      i == Instruction::UIToFP ||
                      i == Instruction::SIToFP ||
                      i == Instruction::FCmp );

  }

  return checked;
}

std::vector<bool> NXTInstAggregateOperation::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(const auto &st : states) {
    Instruction *nextInst = st->pc->inst;
    unsigned int i = nextInst->getOpcode();

    checked.push_back(i == Instruction::MemoryOps::GetElementPtr ||
                      i == Instruction::InsertValue ||
                      i == Instruction::ExtractValue );
  }

  return checked;
}

std::vector<bool> NXTInstVectorOperation::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(const auto &st : states) {
    Instruction *nextInst = st->pc->inst;
    unsigned int i = nextInst->getOpcode();

    checked.push_back(i == Instruction::InsertElement ||
                      i == Instruction::ExtractElement);
  }

  return checked;
}

NXTInstSwitchWithSym::NXTInstSwitchWithSym(Executor &_executor)
  : executor(_executor) {
}

std::vector<bool> NXTInstSwitchWithSym::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(const auto &st : states) {
    KInstruction *ki = st->pc;
    Instruction *i = ki->inst;
    unsigned int opcode = i->getOpcode();

    bool isSwitch = (opcode == Instruction::Switch);
    if (!isSwitch) {
      checked.push_back(false);
    } else {
      ref<Expr> cond = executor.eval(ki, 0, *st).value;
      bool withSym = !(dyn_cast<ConstantExpr>(cond));
      checked.push_back(withSym);
    }
  }

  return checked;
}

NXTInstAllocaWithSym::NXTInstAllocaWithSym(Executor &_executor)
  : executor(_executor) {
}

std::vector<bool> NXTInstAllocaWithSym::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(const auto &st : states) {
    KInstruction *ki = st->pc;
    Instruction *i = ki->inst;
    unsigned int opcode = i->getOpcode();

    bool isAlloca = (opcode == Instruction::Alloca);
    if (!isAlloca) {
      checked.push_back(false);
    } else {
      AllocaInst *ai = cast<AllocaInst>(i);
      unsigned elementSize = 
        (executor.kmodule)->targetData->getTypeStoreSize(ai->getAllocatedType());
      ref<Expr> size = Expr::createPointer(elementSize);
      if (ai->isArrayAllocation()) {
        ref<Expr> count = executor.eval(ki, 0, *st).value;
        count = Expr::createZExtToPointerWidth(count);
        size = MulExpr::create(size, count);
      }
      size = executor.toUnique(*st, size);
      bool withSym = !(dyn_cast<ConstantExpr>(size));
      checked.push_back(withSym);
    }
  }

  return checked;
}

NXTInstIndirectBrWithSym::NXTInstIndirectBrWithSym(Executor &_executor)
  : executor(_executor) {
}

std::vector<bool> NXTInstIndirectBrWithSym::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(const auto &st : states) {
    KInstruction *ki = st->pc;
    Instruction *i = ki->inst;
    unsigned int opcode = i->getOpcode();

    bool isIndirectBr = (opcode == Instruction::IndirectBr);
    if (!isIndirectBr) {
      checked.push_back(false);
    } else {
      ref<Expr> address = executor.eval(ki, 0, *st).value;
      address = executor.toUnique(*st, address);
      bool withSym = !(dyn_cast<ConstantExpr>(address.get()));
      checked.push_back(withSym);
    }
  }

  return checked;
}

// Features related to instruction history
std::vector<bool> SmallestInstructionStepped::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (steppedInstructions, (ExecutionState*, index of state)) sorted by steppedInstructions
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t steppedInstructions = st->steppedInstructions;
    st_set.insert(std::make_pair(steppedInstructions, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

std::vector<bool> SmallestInstructionsSinceCovNew::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (instsSinceCovNew, (ExecutionState*, index of state)) sorted by instsSinceCovNew
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int instsSinceCovNew = st->instsSinceCovNew;
    st_set.insert(std::make_pair(instsSinceCovNew, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

std::vector<bool> SmallestCallPathInstruction::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (CallPathInstructions, (ExecutionState*, index of state)) sorted by CallPathInstructions
  // CallPathInstruction: instructions in currently executing function
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t CPInsts = st->stack.back().callPathNode->statistics.getValue(stats::instructions);
    st_set.insert(std::make_pair(CPInsts, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

// Features related to symbolic memory state
std::vector<bool> SmallestSymbolics::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (symbolics.size(), (ExecutionState*, index of state)) sorted by symbolics.size()
  // with ascending order 
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t symSize = st->symbolics.size();
    st_set.insert(std::make_pair(symSize, std::make_pair(st, i++)));
  }

  //criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

std::vector<bool> LargestSymbolics::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (symbolics.size(), (ExecutionState*, index of state)) sorted by symbolics.size()
  // with descending order 
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<size_t, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st: states) {
    size_t symSize = st->symbolics.size();
    st_set.insert(std::make_pair(symSize, std::make_pair(st, i++)));
  }

  //criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

// Features related to path condition
std::vector<bool> LowestQueryCost::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (queryCost, (ExecutionState*, index of state)) sorted by queryCost
  std::set<std::pair<double, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    double qc = st->queryCost.toSeconds();
    st_set.insert(std::make_pair(qc, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

std::vector<bool> ShallowestState::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (depth, (ExecutionState*, index of state)) sorted by depth with ascending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int depth = st->depth;
    st_set.insert(std::make_pair(depth, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

std::vector<bool> DeepestState::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (depth, (ExecutionState*, index of state)) sorted by depth with descending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int depth = st->depth;
    st_set.insert(std::make_pair(depth, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}

std::vector<bool> ShortestConstraints::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (constraintsSize, (ExecutionState*, index of state)) sorted by constraintsSize
  std::set<std::pair<size_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    size_t constraintsSize = st->constraints.size();
    st_set.insert(std::make_pair(constraintsSize, std::make_pair(st, i++)));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked[(it->second).second] = true;
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked[(it->second).second] = false;
  }

  return checked;
}
