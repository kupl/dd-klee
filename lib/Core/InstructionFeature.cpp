
#include "CoreStats.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "InstructionFeature.h"
#include "StatsTracker.h"

#include "klee/ExecutionState.h"

#include <set>
#include <vector>

using namespace klee;
using namespace llvm;

std::vector<bool> SmallestInstructionsStepped::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (steppedInstructions, (ExecutionState*, index of state)) sorted by steppedInstructions
  // with ascending order
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<uint64_t> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t steppedInstructions = st->steppedInstructions;
    st_set.insert(std::make_pair(steppedInstructions, std::make_pair(st, i++)));
    val_set.insert(steppedInstructions);
  }

  stats::uniqueRatioInstsStepped += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<uint64_t>(st_set, marked);
}

std::vector<bool> LargestInstructionsStepped::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (steppedInstructions, (ExecutionState*, index of state)) sorted by steppedInstructions
  // with descending order
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>>> st_set;

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
  // with ascending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<unsigned int> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int instsSinceCovNew = st->instsSinceCovNew;
    st_set.insert(std::make_pair(instsSinceCovNew, std::make_pair(st, i++)));
    val_set.insert(instsSinceCovNew);
  }

  stats::uniqueRatioInstsSinceCovNew += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<unsigned int>(st_set, marked);
}

std::vector<bool> LargestInstructionsSinceCovNew::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (instsSinceCovNew, (ExecutionState*, index of state)) sorted by instsSinceCovNew
  // with descending order
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>>> st_set;

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
  // with ascending order
  // CallPathInstruction: instructions in currently executing function
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<uint64_t> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t CPInsts = st->stack.back().callPathNode->statistics.getValue(stats::instructions);
    st_set.insert(std::make_pair(CPInsts, std::make_pair(st, i++)));
    val_set.insert(CPInsts);
  }

  stats::uniqueRatioCPInsts += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<uint64_t>(st_set, marked);
}

std::vector<bool> LargestCallPathInstructions::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (CallPathInstructions, (ExecutionState*, index of state)) sorted by CallPathInstructions
  // with descending order
  // CallPathInstruction: instructions in currently executing function
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>>> st_set;

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
  // with ascending order
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  std::set<uint64_t> val_set;

  size_t i = 0;
  for(const auto &st : states) {
    StackFrame &sf = st->stack.back();
    uint64_t md2u = computeMinDistToUncovered(st->pc, sf.minDistToUncoveredOnReturn);
    st_set.insert(std::make_pair(md2u, std::make_pair(st, i++)));
    val_set.insert(md2u);
  }

  stats::uniqueRatioMD2U += (uint64_t) ((double) val_set.size() / states.size() * 100.0);

  return markFeature<uint64_t>(st_set, marked);
}

std::vector<bool> FarthestToUncoveredInstruction::operator()(
    const std::vector<ExecutionState*> &states,
    std::vector<bool> &marked) {
  // (md2u, (ExecutionState*, index of state)) sorted by md2u
  // with descending order
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>,
           std::greater<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    StackFrame &sf = st->stack.back();
    uint64_t md2u = computeMinDistToUncovered(st->pc, sf.minDistToUncoveredOnReturn);
    st_set.insert(std::make_pair(md2u, std::make_pair(st, i++)));
  }

  return markFeature<uint64_t>(st_set, marked);
}