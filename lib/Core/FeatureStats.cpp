
#include "FeatureStats.h"

using namespace klee;

Statistic stats::featureExtractionTotal("FeatureExtractionTotal", "FETotal");
Statistic stats::featureExtractionFork("FeatureExtractionFork", "FEFork");
Statistic stats::featureExtractionTermination("FeatureExtractionTermination", "FETermination");
Statistic stats::featureExtractionCall("FeatureExtractionCall", "FECall");
Statistic stats::featureExtractionReturn("FeatureExtractionReturn", "FEReturn");
