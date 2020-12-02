
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
    std::map<ExecutionState*, std::vector<double>> raw_fv_map;

    Executor &executor;
    unsigned totalFeatureMaps;
  public:
    FeatureHandler(Executor &executor,
                   const std::vector<ExecutionState*> &states,
                   const std::string &weightFile);
    ~FeatureHandler();
    void extractFeatures(const std::vector<ExecutionState*> &states);
    ExecutionState* getTop(const std::vector<ExecutionState*> &states);
    
    void normalizeFeature(const std::set<std::pair<double, ExecutionState*>> &st_set);
    std::string getFeatureMapFilename(unsigned id);
};

} // End klee namespace

#endif /* KLEE_FEATUREHANDLER_H */
