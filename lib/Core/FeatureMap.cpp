
#include "FeatureMap.h"

#include "klee/ExecutionState.h"

#include <map>
#include <numeric>
#include <fstream>
#include <vector>

FeatureMap::FeatureMap(const std::vector<ExecutionState*> &states,
                       const std::string &weightFile,
                       Executor &_executor)
  : executor(_executor) {
  double weight;
  std::ifstream win(weightFile.c_str());

  assert(win && "no weight file");
  while (win >> weight)
    weights.push_back(weight);
  
  features.push_back(new NXTInstExternalFunctionCall());
  features.push_back(new NXTInstFPOperation());
  features.push_back(new NXTInstAggregateOperation());
  features.push_back(new NXTInstVectorOperation());
  features.push_back(new NXTInstSwitchWithSym(executor));
  features.push_back(new NXTInstAllocaWithSym(executor));
  features.push_back(new NXTInstIndirectBrWithSym(executor));

  features.push_back(new SmallestInstructionStepped());
  features.push_back(new SmallestInstructionsSinceCovNew());
  features.push_back(new SmallestCallPathInstruction());
  features.push_back(new ClosestToUncoveredInst());

  features.push_back(new SmallestAddressSpace());
  features.push_back(new LargestAddressSpace());
  features.push_back(new SmallestSymbolics());
  features.push_back(new LargestSymbolics());
  features.push_back(new HighestNumOfConstExpr());
  features.push_back(new HighestNumOfSymExpr());

  features.push_back(new SmallestNumOfSymbolicBranches());
  features.push_back(new HighestNumOfSymbolicBranches());
  features.push_back(new LowestQueryCost());
  features.push_back(new ShallowestState());
  features.push_back(new DeepestState());
  features.push_back(new ShortestConstraints());

  featureCount = features.size();

  assert(featureCount == (int)weights.size() && "weight size error");
}

FeatureMap::~FeatureMap() {}

void FeatureMap::updateMap(const std::vector<ExecutionState*> &states) {
  fv_map.clear();
  for(const auto f : features) {
    checkedStates = (*f)(states);
    int statesCount = checkedStates.size();
    assert(statesCount == (int)states.size() && "undesired behavior in feature extraction");
    for(int i = 0; i < statesCount; i++) {
      fv_map[states[i]].push_back(checkedStates[i]);
    }
  }
}

ExecutionState* FeatureMap::getTop(const std::vector<ExecutionState*> &states) {
  ExecutionState* topState = 0;
  double topScore = -100000000;
  for(const auto &fvector : fv_map) {
    const auto &v = fvector.second;
    double score = std::inner_product(v.begin(), v.end(), weights.begin(), 0);
    if (score > topScore) {
      topState = fvector.first;
      topScore = score;
    }
  }

  return topState;
}
	