/** @file runAnalysis.cpp
 *  @brief Main function for analysis
 *
 *  Figure out which config to use.
 *  Create the AnalysisManager and any
 *  secondary Analysis, add them to the manager
 *  and runs the analysis.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#include "YKAnalysis/AnalysisManager.h"
#include "YKAnalysis/BaseAnalysis.h"
#include "ClusterAnalysis/FluctuationAnalysis.h"
#include "JetAnalysis/JetAnalysis.h"
#include "OverlayAnalysis/OverlayAnalysis.h"

#include <TString.h>

using namespace std;

int main( int argc, char* argv[] ){

  // See if we have a config file specified
  // otherwise, use default
  TString cfgName = "config/config.cfg";
  if( argc == 2 ){
    TString arg;
    arg = TString( argv[1] );
    if( arg.Contains("config") || arg.Contains("cfg") )
      cfgName = arg;
  }
    
  std::string outputName = "myOut.root";
  // Create Analysis manager
  YKAnalysis::AnalysisManager* manager =
    new YKAnalysis::AnalysisManager( outputName.c_str(), cfgName.Data() );
 
  // should add BaseAnalysis. 
  // Leave it here in case decide to add options later
  // If you do add it, add it first, because it has event selection -
  // things like trigger decision, and you dont want to run the other 
  // analysis in case you dont pass trigger (or maybe you do, depends)
  manager->AddAnalysis( make_shared<YKAnalysis::BaseAnalysis>() );

  // add JetAnalysis. 
  manager->AddAnalysis( make_shared<JetAnalysis::JetAnalysis>() );

  // add FluctuationAnalysis. 
  // manager->AddAnalysis( make_shared<ClusterAnalysis::FluctuationAnalysis>() );

  // overlay analysis
  // manager->AddAnalysis( make_shared<OverlayAnalysis::OverlayAnalysis>() );

  // Run
  // manager->SetMaxEvents( 5000 );
  manager->Run();

  return 0;  
}
