
#ifndef KLEE_FEATURESTATS_H
#define KLEE_FEATURESTATS_H

#include "klee/Statistic.h"

namespace klee {
namespace stats {

  /// The number of feature extractions for each situation
  extern Statistic featureExtractionTotal;
  extern Statistic featureExtractionFork;
  extern Statistic featureExtractionTermination;
  extern Statistic featureExtractionCall;
  extern Statistic featureExtractionReturn;
  
}
}

#endif /* KLEE_FEATURESTATS_H */
