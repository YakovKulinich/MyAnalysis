/** @file AnalysisManager.cxx
 *  @brief Implementation of AnalysisManager.
 *
 *  Function definitions for AnalysisManager  are provided. 
 *  This class is the main  class for the analysis.
 *  It creates the  shared data, initializes basic tools
 *  such as GRL, trigger decision, and vertex tools.
 *  After checking those, it calls subsequent algorithms
 *  to execute.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#include "YKAnalysis/Global.h"

#include "YKAnalysis/AnalysisManager.h"
#include "YKAnalysis/Analysis.h"
#include "YKAnalysis/SharedData.h"

#include <TFile.h>
#include <TEnv.h>
#include <TChain.h>
#include <TSystem.h>

#include <iostream>
#include <fstream>
#include <sstream>


/** @brief Default Constructor for AnalysisManager.
 */
YKAnalysis :: AnalysisManager :: AnalysisManager () 
  : m_analysisName  ( "" ),
    m_inputTreeName ( "" ),
    m_eventChain    (NULL) 
{ 
  m_sd = new SharedData(); 
}


/** @brief Constructor for AnalysisManager.
 *
 *  Constructor with arguments.
 *
 *  @param Name of configuration file
 */
YKAnalysis :: AnalysisManager :: AnalysisManager ( const std::string& outputFileName,
						   const std::string& configFileName ) 
  : m_analysisName  ( "runAnalysis" ), 
    m_inputTreeName ( "CollectionTree" ),
    m_eventChain    (NULL) 
{
  m_sd             = new SharedData( outputFileName.c_str(), configFileName.c_str() );
}

/** @brief Destructor for AnalysisManager.
 *
 *  Cleans up an AnalysisManager object.
 */
YKAnalysis :: AnalysisManager :: ~AnalysisManager()
{
  delete m_sd;
}

/** @brief Adds an algorithm to manager's vector of algorithms.
 *
 *  Adds a pointer to an algorithm to the vector
 *  of algorithms controlled by the manager.
 * 
 *  Return int not xAOD::TReturnCode to avoid steering macro
 *  having to perform CHECK_STATUS.
 *
 *  @param Analysis smart pointer
 *  @return TReturnCode::kSuccess (which is just 1) 
 */
int YKAnalysis :: AnalysisManager :: AddAnalysis ( AnalysisPtr ana )
{
  m_v_analysis.push_back( ana );
  m_v_analysis.back()->RegisterSharedData( m_sd );
  return xAOD::TReturnCode::kSuccess;
}

/** @brief Run method for manager.
 *  
 *  Has sequence of commands for manager to Run.
 *  setup() -> histInitialize() -> initialize() -> 
 *  execute() -> finalize() -> histFinalize(). 
 *
 *  Return int not xAOD::TReturnCode to avoid steering macro
 *  having to perform CHECK_STATUS.
 *
 *  @return TReturnCode::kSuccess (which is just 1)
 */
int YKAnalysis :: AnalysisManager :: Run () 
{
  std::cout << m_analysisName << " Running" << std::endl;
  
  CHECK_STATUS( Form("%s::Run", m_analysisName.c_str() ), Setup() );
  for( auto& ana : m_v_analysis ){
    CHECK_STATUS( Form("%s::Run", ana->GetAnalysisName().c_str() ), ana->Setup() );
    CHECK_STATUS( Form("%s::Run", ana->GetAnalysisName().c_str() ), ana->HistInitialize() );
    CHECK_STATUS( Form("%s::Run", ana->GetAnalysisName().c_str() ), ana->Initialize() );
  }

  // RUN EVENT LOOP
  CHECK_STATUS( Form("%s::Run", m_analysisName.c_str() ), EventLoop() );
  // FINISH RUNNING EVENT LOOP

  for( auto& ana : m_v_analysis ){
    CHECK_STATUS( Form("%s::Run", ana->GetAnalysisName().c_str() ), ana->Finalize() );  
    CHECK_STATUS( Form("%s::Run", ana->GetAnalysisName().c_str() ), ana->HistFinalize() );
  }

  m_sd->Finalize();

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Setup method for manager.
 *
 *  Sets up xAOD access. A new eventStore is created
 *  as part of SharedData's AddEventStore. 
 *  I.e. the pointer from the new operator is passed there.
 *  Opens input files/samples.
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode YKAnalysis :: AnalysisManager :: Setup ()
{
  std::cout << m_analysisName << " Setup" << std::endl; 

  //-----------------
  //  Initialize SD
  //-----------------
  m_sd->Initialize();

  //-----------------
  //  Configs
  //-----------------
  TEnv* config = m_sd->GetConfig();

  std::string inputFileName = config->GetValue( "inputFileName", "" );
  int         runMode       = config->GetValue( "runMode" , 0 );

  //-----------------
  //  xAOD EventStore
  //-----------------
  xAOD::TReturnCode::enableFailure();
  CHECK_STATUS( Form("%s::setup", m_analysisName.c_str() ),
		xAOD::Init("Initializing") );

  m_sd->AddEventStore( new xAOD::TEvent( xAOD::TEvent::kClassAccess ) );

  //-----------------
  //   Input files
  //-----------------
  std::vector< std::string > inputFileList;

  if( runMode == 1 ) {
    // running on grid, use pruns generated inputFiles.txt
    // which is just comma separated list of files
    std::ifstream ifs("inputFiles.txt");
    std::string line, entry;
    while ( getline( ifs, line ) ){
      std::istringstream ss( line );
      while ( getline( ss, entry, ',' ) ) 
	inputFileList.push_back( entry );
    }
    if ( !ifs.eof() ) std::cout << "Naht Good!\n" << std::endl;
  }
  else if( runMode == 0 && inputFileName != "" ){
    // not grid, check if there is an input file given
    inputFileList.push_back( inputFileName ); 
  }
  else{
    // No input file specified, not on grid so no inputFiles.txt
    // Nothing to run on, so quit.
    std::cout << "No input files specified. Exiting." << std::endl;
    return xAOD::TReturnCode::kSuccess; 
  }
  
  // produce a root chain with all the files
  m_eventChain = new TChain(m_inputTreeName.c_str());
  for( const auto& inputFile : inputFileList ){
    std::cout << "open " << inputFile << std::endl;
    m_eventChain->Add( inputFile.c_str() );
  }

  xAOD::TEvent* eventStore = m_sd->GetEventStore();
  CHECK_STATUS( Form("%s::setup",m_analysisName.c_str() ),
		eventStore->readFrom( m_eventChain ) );
  std::cout << "There are " << eventStore->getEntries() << " events" << std::endl;

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Event Loop method for manager.
 *
 *  Runs the "main" event loop. 
 *  Calls its own ProcessEvents and 
 *  ProcessEvents for other algorithms
 *  Checks if they ran successfully. 
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode YKAnalysis :: AnalysisManager :: EventLoop () 
{
  std::cout << "Entering Event Loop..." << std::endl;

  xAOD::TEvent* eventStore = m_sd->GetEventStore();
  int nevents =  eventStore->getEntries();  

  std::cout << m_analysisName << " Executing" << " with " << nevents << " events."<< std::endl;

  // EVENT LOOP
  for( int ev = 0; ev < nevents; ev++ ){
    bool isGoodEvent = true;

    eventStore->getEntry( ev );
    if( m_sd->DoPrint() ) std::cout << "\nSampleEvent : " << m_sd->GetEventCounter() << std::endl;

    for( auto& ana : m_v_analysis ){ 
      if( ana->ProcessEvent() !=  xAOD::TReturnCode::kSuccess ) isGoodEvent = false;
    }

    if( !isGoodEvent ) continue;

    m_sd->EndOfEvent();
  } // END EVENT LOOP

  return xAOD::TReturnCode::kSuccess;
}
