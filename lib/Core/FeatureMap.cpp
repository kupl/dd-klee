
#include "FeatureMap.h"

#include "klee/ExecutionState.h"

#include <map>
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

  featureCount = features.size();
}

FeatureMap::~FeatureMap() {}

void FeatureMap::updateMap(const std::vector<ExecutionState*> &states) {
  for(auto f : features) {
    checkedStates = (*f)(states);
    int statesCount = checkedStates.size();
    assert(statesCount == states.size() && "undesired behavior in feature extraction");
    for(int i = 0; i < statesCount; i++) {
      fv_map[states[i]].push_back(checkedStates[i]);
    }
  }
}

ExecutionState* FeatureMap::getTop(const std::vector<ExecutionState*> &states) {
  // PARAM_TODO : Implement Y = Wx
	return states.back();
}
	
