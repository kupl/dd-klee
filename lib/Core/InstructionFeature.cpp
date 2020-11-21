
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

std::set<std::pair<double, ExecutionState*>>
FInstructionsStepped::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    double steppedInstructions = (double)st->steppedInstructions;
    st_set.insert(std::make_pair(steppedInstructions, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FInstructionsSinceCovNew::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    double instsSinceCovNew = (double)st->instsSinceCovNew;
    st_set.insert(std::make_pair(instsSinceCovNew, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FCallPathInstructions::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    double CPInsts = (double)st->stack.back().callPathNode->statistics.getValue(stats::instructions);
    st_set.insert(std::make_pair(CPInsts, st));
  }

  return st_set;
}

std::set<std::pair<double, ExecutionState*>>
FMinDistToUncoveredInst::operator()(const std::vector<ExecutionState*> &states) {
  std::set<std::pair<double, ExecutionState*>> st_set;

  for(const auto &st : states) {
    StackFrame &sf = st->stack.back();
    double md2u = (double)computeMinDistToUncovered(st->pc, sf.minDistToUncoveredOnReturn);
    st_set.insert(std::make_pair(md2u, st));
  }

  return st_set;
}
