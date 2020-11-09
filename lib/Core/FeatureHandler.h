
#ifndef KLEE_FEATUREHANDLER_H
#define KLEE_FEATUREHANDLER_H

#include "klee/ExecutionState.h"

#include "Feature.h"

#include <map>
#include <vector>

using namespace klee;

namespace klee {
  class Executor;

class FeatureHandler {
    int featureCount;
    std::vector<Feature*> features;
    std::vector<double> weights;
    std::map<ExecutionState*, std::vector<int>> fv_map;
  
  public:
    FeatureHandler(const std::vector<ExecutionState*> &states,
                   const std::string &weightFile);
    ~FeatureHandler();
    void extractFeatures(const std::vector<ExecutionState*> &states);
    ExecutionState* getTop(const std::vector<ExecutionState*> &states);
};

} // End klee namespace

#endif /* KLEE_FEATUREHANDLER_H */
