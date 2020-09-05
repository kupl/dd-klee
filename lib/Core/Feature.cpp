
#include "CallPathManager.h"
#include "CoreStats.h"
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
  std::vector<bool> checked(states.size());

  // (steppedInstructions, (ExecutionState*, index of state)) sorted by steppedInstructions
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    st_set.insert(std::make_pair(st->steppedInstructions, std::make_pair(st, i++)));
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
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    st_set.insert(std::make_pair(st->instsSinceCovNew, std::make_pair(st, i++)));
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

// Features related to path condition
std::vector<bool> LowestQueryCost::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked(states.size());

  // (queryCost, (ExecutionState*, index of state)) sorted by queryCost
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

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
