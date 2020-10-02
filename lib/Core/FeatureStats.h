
#ifndef KLEE_FEATURESTATS_H
#define KLEE_FEATURESTATS_H

#include "klee/Statistic.h"

namespace klee {
namespace stats {

  /// Number of feature extraction by ParameterizedSearcher
  extern Statistic featureExtractions;
  
  /// Number of added and removed states when extracting new feature
  extern Statistic addedStatesFE;
  extern Statistic removedStatesFE;

}
}

#endif /* KLEE_FEATURESTATS_H */
