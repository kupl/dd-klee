
#ifndef KLEE_FEATUREMAP_H
#define KLEE_FEATUREMAP_H

#include "klee/ExecutionState.h"

#include "Feature.h"

#include <map>
#include <vector>

using namespace klee;

namespace klee {
  class Executor;

class FeatureMap {
    const Executor &executor;
    int featureCount;
    std::vector<Feature*> features;
    std::vector<double> weights;
    std::map<ExecutionState*, std::vector<int>> fv_map;
    std::vector<bool> checkedStates;
  
  public:
    FeatureMap(const std::vector<ExecutionState*> &states,
               const std::string &weightFile,
               const Executor &_executor);
    ~FeatureMap();
    void updateMap(const std::vector<ExecutionState*> &states);
    ExecutionState* getTop(const std::vector<ExecutionState*> &states);
};

} // End klee namespace

#endif /* KLEE_FEATUREMAP_H */
