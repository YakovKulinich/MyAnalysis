/** @file runAnalysis.cpp
 *  @brief Main function for analysis
 *
 *  Figure out which config to use.
 *
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
  manager->AddAnalysis( make_shared<YKAnalysis::BaseAnalysis>() );

  // should add JetAnalysis. 
  manager->AddAnalysis( make_shared<JetAnalysis::JetAnalysis>() );

  // should add FluctuationAnalysis. 
  // manager->AddAnalysis( make_shared<ClusterAnalysis::FluctuationAnalysis>() );


  // Run
  manager->SetMaxEvents( 5000 );
  manager->Run();

  return 0;  
}
