
#include "FeatureStats.h"

using namespace klee;

Statistic stats::featureExtractions("FeatureExtractions", "FE");

Statistic stats::uniqueRatioInstsStepped("UniqueRationInstsStepped", "URInstsStepped");
Statistic stats::uniqueRatioInstsSinceCovNew("UniqueRationInstsSinceCovNew", "URInstsSinceCovNew");
Statistic stats::uniqueRatioCPInsts("UniqueRatioCPInsts", "URCPInsts");
Statistic stats::uniqueRatioMD2U("UniqueRatioMD2U", "URMD2U");
Statistic stats::uniqueRatioAddrSpace("UniqueRatioAddrSpace", "URAddrSpace");
Statistic stats::uniqueRatioSymbolics("UniqueRatioSymbolics", "URSymbolics");
Statistic stats::uniqueRatioConcreteExprCount("UniqueRatioConcreteExprCount", "URConcreteExprCnt");
Statistic stats::uniqueRatioSymbolicExprCount("UniqueRatioSymExprCount", "URSymExprCnt");
Statistic stats::uniqueRatioSymBrCount("UniqueRatioSymBrCount", "URSymBrCnt");
Statistic stats::uniqueRatioQC("UniqueRatioQC", "URQC");
Statistic stats::uniqueRatioDepth("UniqueRatioDepth", "URDepth");
Statistic stats::uniqueRatioConstraints("UniqueRatioConstraints", "URConstraints");
Statistic stats::featureExtractionFork("FeatureExtractionFork", "FEFork");
Statistic stats::featureExtractionTermination("FeatureExtractionTermination", "FETermination");
Statistic stats::featureExtractionCall("FeatureExtractionCall", "FECall");
Statistic stats::featureExtractionReturn("FeatureExtractionReturn", "FEReturn");
