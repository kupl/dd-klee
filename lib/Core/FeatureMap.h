
#ifndef KLEE_FEATUREMAP_H
#define KLEE_FEATUREMAP_H

#include "klee/ExecutionState.h"

#include "Feature.h"

#include <map>
#include <vector>

using namespace klee;

class FeatureMap {
    int featureCount;
    std::vector<Feature*> features;
    std::vector<double> weights;
    std::map<ExecutionState*, std::vector<int>> fv_map;
    std::vector<bool> checkedStates;
  
  public:
    FeatureMap(const std::vector<ExecutionState*> &states,
                     const std::string &weightFile);
    ~FeatureMap();
    void updateMap(const std::vector<ExecutionState*> &states);
    ExecutionState* getTop(const std::vector<ExecutionState*> &states);
};

#endif /* KLEE_FEATUREMAP_H */
