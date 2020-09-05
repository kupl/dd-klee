
#include "FeatureMap.h"

#include "klee/ExecutionState.h"

#include <map>
#include <numeric>
#include <fstream>
#include <vector>

FeatureMap::FeatureMap(const std::vector<ExecutionState*> &states,
                                   const std::string &weightFile) {
  double weight;
  std::ifstream win(weightFile.c_str());

  assert(win && "no weight file");
  while (win >> weight)
    weights.push_back(weight);
  
  features.push_back(new NXTInstExternalFunctionCall());
  features.push_back(new NXTInstFPOperation());
  features.push_back(new NXTInstAggregateOperation());
  features.push_back(new NXTInstVectorOperation());

  features.push_back(new SmallestInstructionStepped());
  features.push_back(new SmallestInstructionsSinceCovNew());
  features.push_back(new SmallestCallPathInstruction());

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
	
