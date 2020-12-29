
#include "ConstraintFeature.h"
#include "Executor.h"
#include "FeatureHandler.h"
#include "FeatureStats.h"
#include "InstructionFeature.h"
#include "StatsTracker.h"
#include "SymMemoryFeature.h"

#include "klee/ExecutionState.h"
#include "klee/Internal/Support/FileHandling.h"

#include "llvm/Support/CommandLine.h"

#include <algorithm>
#include <map>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace klee;
using namespace llvm;

///

namespace {
cl::OptionCategory
    FeatureMapCat("Feature map options",
                  "This option controls the period of recording feature map generated by feature handler");

cl::opt<unsigned> WriteFeatureMapAfterFeatureExtractions(
    "write-feature-map-after", cl::init(0),
    cl::desc(
        "Write feature map to csv file after eacn n feature extractions, "
        "0 to disable (default = 0)"),
    cl::cat(FeatureMapCat));
} // namespace

///

FeatureHandler::FeatureHandler(Executor &_executor,
                               const std::vector<ExecutionState*> &states,
                               const std::string &weightFile) :
    executor(_executor), totalFeatureMaps(0) {
  double weight;
  std::ifstream win(weightFile.c_str());

  assert(win && "no weight file");
  while (win >> weight)
    weights.push_back(weight);

  // mark top, bottom states for each element of features
  features.push_back(new FInstructionsStepped());
  features.push_back(new FInstructionsSinceCovNew());
  features.push_back(new FCallPathInstructions());
  features.push_back(new FMinDistToUncoveredInst());

  features.push_back(new FAddressSpace());
  features.push_back(new FSymbolics());
  features.push_back(new FNumOfConstExpr());
  features.push_back(new FNumOfSymExpr());

  features.push_back(new FQueryCost());
  features.push_back(new FDepth());
  features.push_back(new FConstraints());
  features.push_back(new FCallPathLoopDepth());
  features.push_back(new FCallerLoopDepth());

  featureCount = features.size();

  assert(featureCount * 2 == (int)weights.size() && "weight size error");
}

FeatureHandler::~FeatureHandler() {}

void FeatureHandler::extractFeatures(const std::vector<ExecutionState*> &states) {
  fv_map.clear();
  raw_fv_map.clear();
  
  int bot = (int) states.size() * Feature::criterion;
  int top = states.size() - bot;
  for(const auto f : features) {
    std::set<std::pair<double, ExecutionState*>> fvalues = (*f)(states);

    int i = 0;
    for(const auto &fvalues_el: fvalues) {
      if(i < bot) {
        fv_map[fvalues_el.second].push_back(1);
        fv_map[fvalues_el.second].push_back(0);
      } else if (i < top) {
        fv_map[fvalues_el.second].push_back(0);
        fv_map[fvalues_el.second].push_back(0);
      } else {
        fv_map[fvalues_el.second].push_back(0);
        fv_map[fvalues_el.second].push_back(1);
      }
      i++;
    }

    if (WriteFeatureMapAfterFeatureExtractions &&
        stats::featureExtractions % WriteFeatureMapAfterFeatureExtractions.getValue() == 0)
      normalizeFeature(fvalues);  
  }

  if (WriteFeatureMapAfterFeatureExtractions &&
      stats::featureExtractions % WriteFeatureMapAfterFeatureExtractions.getValue() == 0) {
    unsigned id = ++totalFeatureMaps;
    std::string path = 
      executor.interpreterHandler->getOutputFilename(getFeatureMapFilename(id));
    std::string error;
    auto f = klee_open_output_file(path, error);

    for (const auto &fvector : raw_fv_map) {
      int i;
      const auto &v = fvector.second;
      i = 0;
      for(const auto &value : v) {
        *f << value;
        if (i++ < featureCount - 1)
          *f << ", ";
      }
      *f << "\n";
    }
  }
  ++stats::featureExtractions;
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

void FeatureHandler::normalizeFeature(const std::set<std::pair<double, ExecutionState*>> &fvalues) {
  double max_value = fvalues.rbegin()->first;
  double min_value = fvalues.begin()->first;

  // min-max normalization for feature values
  for(const auto &fvalues_el : fvalues) {
    if(max_value != min_value) {
      raw_fv_map[fvalues_el.second].push_back((double) (fvalues_el.first - min_value) / (max_value - min_value));
    } else {
      raw_fv_map[fvalues_el.second].push_back(0.0);
    }
  }
}
  
  
std::string
FeatureHandler::getFeatureMapFilename(unsigned id) {
  std::stringstream filename;
  filename << "fmap" << std::setfill('0') << std::setw(6) << id << ".csv";
  return filename.str();
 }	