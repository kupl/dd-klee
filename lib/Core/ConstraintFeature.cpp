
#include "ConstraintFeature.h"
#include "Feature.h"
#include "FeatureStats.h"
#include "StatsTracker.h"

#include "klee/ExecutionState.h"

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

  return markFeature<double>(st_set, marked);
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

  return markFeature<size_t>(st_set, marked);
}
