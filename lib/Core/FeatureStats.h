
#ifndef KLEE_FEATURESTATS_H
#define KLEE_FEATURESTATS_H

#include "klee/Statistic.h"

namespace klee {
namespace stats {

  /// Number of feature extraction by ParameterizedSearcher
  extern Statistic featureExtractions;

  /// Number of mark of each feature when extracting features
  extern Statistic nextInstExternalFunctionCall;
  extern Statistic nextInstFPOperation;
  extern Statistic nextInstAggregateOperation;
  extern Statistic nextInstVectorOperation;
  extern Statistic nextInstSwitchWithSym;
  extern Statistic nextInstAllocaWithSym;
  extern Statistic nextInstStoreWithSym;
  extern Statistic nextInstIndirectBrWithSym;

}
}

#endif /* KLEE_FEATURESTATS_H */
