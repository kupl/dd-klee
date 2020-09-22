
#include "FeatureStats.h"

using namespace klee;

Statistic stats::featureExtractions("FeatureExtractions", "FExtractions");

Statistic stats::nextInstExternalFunctionCall("Feature01", "F01");
Statistic stats::nextInstFPOperation("Feature02", "F02");
Statistic stats::nextInstAggregateOperation("Feature03", "F03");
Statistic stats::nextInstVectorOperation("Feature04", "F04");
Statistic stats::nextInstSwitchWithSym("Feature05", "F05");
Statistic stats::nextInstAllocaWithSym("Feature06", "F06");
Statistic stats::nextInstStoreWithSym("Feature07", "F07");
Statistic stats::nextInstIndirectBrWithSym("Feature08", "F08");
