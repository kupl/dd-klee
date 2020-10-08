
#include "FeatureHandler.h"

#include "klee/ExecutionState.h"

#include <map>
#include <numeric>
#include <fstream>
#include <vector>

FeatureHandler::FeatureHandler(const std::vector<ExecutionState*> &states,
                       const std::string &weightFile,
                       Executor &_executor)
  : executor(_executor) {
  double weight;
  std::ifstream win(weightFile.c_str());

  assert(win && "no weight file");
  while (win >> weight)
    weights.push_back(weight);

  features.push_back(new SmallestInstructionsStepped());
  features.push_back(new LargestInstructionsStepped());
  features.push_back(new SmallestInstructionsSinceCovNew());
  features.push_back(new LargestInstructionsSinceCovNew());
  features.push_back(new SmallestCallPathInstructions());
  features.push_back(new LargestCallPathInstructions());
  features.push_back(new ClosestToUncoveredInstruction());
  features.push_back(new FarthestToUncoveredInstruction());

  features.push_back(new SmallestAddressSpace());
  features.push_back(new LargestAddressSpace());
  features.push_back(new SmallestSymbolics());
  features.push_back(new LargestSymbolics());
  features.push_back(new HighestNumOfConstExpr());
  features.push_back(new HighestNumOfSymExpr());

  features.push_back(new LowestQueryCost());
  features.push_back(new HighestQueryCost());
  features.push_back(new ShallowestState());
  features.push_back(new DeepestState());
  features.push_back(new ShortestConstraints());
  features.push_back(new LongestConstraints());

  featureCount = features.size();

  assert(featureCount == (int)weights.size() && "weight size error");
}

FeatureHandler::~FeatureHandler() {}

void FeatureHandler::updateMap(const std::vector<ExecutionState*> &states) {
  std::vector<bool> marked(states.size(), false);
  fv_map.clear();
  for(const auto f : features) {
    (*f)(states, marked);
    int statesCount = marked.size();
    assert(statesCount == (int)states.size() && "undesired behavior in feature extraction");
    for(int i = 0; i < statesCount; i++) {
      fv_map[states[i]].push_back(marked[i]);
    }
  }
}

ExecutionState* FeatureHandler::getTop(const std::vector<ExecutionState*> &states) {
  ExecutionState* topState = 0;
  double topScore = -100000000;
  for(const auto &fvector : fv_map) {
    const auto &v = fvector.second;
    double score = std::inner_product(v.begin(), v.end(), weights.begin(), 0.0);
    if (score > topScore) {
      topState = fvector.first;
      topScore = score;
    }
  }

  return topState;
}
	
