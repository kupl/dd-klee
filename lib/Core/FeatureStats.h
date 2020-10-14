
#ifndef KLEE_FEATURESTATS_H
#define KLEE_FEATURESTATS_H

#include "klee/Statistic.h"

namespace klee {
namespace stats {

  /// Number of feature extraction by ParameterizedSearcher
  extern Statistic featureExtractions;
  /// Number of added and removed states when extracting new feature
  extern Statistic featureExtractionFork;
  extern Statistic featureExtractionTermination;
  extern Statistic featureExtractionCall;
  extern Statistic featureExtractionReturn;
  
  /// Number of unique ratio of values for each statistic feature (%)
  extern Statistic uniqueRatioInstsStepped;
  extern Statistic uniqueRatioInstsSinceCovNew;
  extern Statistic uniqueRatioCPInsts;
  extern Statistic uniqueRatioMD2U;
  extern Statistic uniqueRatioAddrSpace;
  extern Statistic uniqueRatioSymbolics;
  extern Statistic uniqueRatioConcreteExprCount;
  extern Statistic uniqueRatioSymbolicExprCount;
  extern Statistic uniqueRatioSymBrCount;
  extern Statistic uniqueRatioQC;
  extern Statistic uniqueRatioDepth;
  extern Statistic uniqueRatioConstraints;

}
}

#endif /* KLEE_FEATURESTATS_H */
