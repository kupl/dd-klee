
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

std::vector<double>
FInstructionsStepped::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t steppedInstructions = st->steppedInstructions;
    st_set.insert(std::make_pair(steppedInstructions, std::make_pair(st, i++)));
  }

  return normalizeFeature<uint64_t>(st_set);
}

std::vector<double>
FInstructionsSinceCovNew::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<unsigned int, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    unsigned int instsSinceCovNew = st->instsSinceCovNew;
    st_set.insert(std::make_pair(instsSinceCovNew, std::make_pair(st, i++)));
  }

  return normalizeFeature<unsigned int>(st_set);
}

std::vector<double> 
FCallPathInstructions::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    uint64_t CPInsts = st->stack.back().callPathNode->statistics.getValue(stats::instructions);
    st_set.insert(std::make_pair(CPInsts, std::make_pair(st, i++)));
  }

  return normalizeFeature<uint64_t>(st_set);
}

std::vector<double>
FMinDistToUncoveredInst::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<uint64_t, std::pair<ExecutionState*, size_t>>> st_set;

  size_t i = 0;
  for(const auto &st : states) {
    StackFrame &sf = st->stack.back();
    uint64_t md2u = computeMinDistToUncovered(st->pc, sf.minDistToUncoveredOnReturn);
    st_set.insert(std::make_pair(md2u, std::make_pair(st, i++)));
  }

  return normalizeFeature<uint64_t>(st_set);
}
