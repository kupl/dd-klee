
#include "Feature.h"

#include "klee/ExecutionState.h"
#include "klee/Internal/Module/InstructionInfoTable.h"
#include "klee/Internal/Module/KInstruction.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"

#include <map>
#include <vector>

using namespace klee;
using namespace llvm;

// PARAM_TODO: categorize features and modularize calculation

// Features related to next instruction
std::vector<bool> NXTInstExternalFunctionCall::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;
  
  for(auto st : states) {
    Instruction *nextInst = st->pc->inst;
    unsigned int i = nextInst->getOpcode();
    
    checked.push_back((i == Instruction::Invoke) || (i == Instruction::Call));
  }

  return checked;
}

std::vector<bool> NXTInstFPOperation::operator()(const std::vector<ExecutionState*> &states) {
  std::vector<bool> checked;

  for(auto st : states) {
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

