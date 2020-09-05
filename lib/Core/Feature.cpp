
#include "Feature.h"

#include "klee/ExecutionState.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"

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

// Features related to instruction history
std::vector<bool> SmallestInstructionStepped::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  // (steppedInstructions, ExecutionState*) sorted by steppedInstructions
  std::set<std::pair<uint64_t, ExecutionState*>> st_set;

  for(const auto &st : states) {
    st_set.insert(std::make_pair(st->steppedInstructions, st));
  }

  // criterion: 10%
  auto boundary = st_set.cbegin();
  std::advance(boundary, st_set.size() * 0.1);

  for(auto it = st_set.cbegin(); it != boundary; it++) {
    checked.push_back(true);
  }
  for(auto it = boundary; it != st_set.cend(); it++) {
    checked.push_back(false);
  }

  return checked;
}