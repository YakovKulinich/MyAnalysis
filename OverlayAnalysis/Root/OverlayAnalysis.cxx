/** @file OverlayAnalysis.cxx
 *  @brief Implementation of OverlayAnalysis.
 *
 *  Function definitions for OverlayAnalysis  are provided. 
 *  It initializes tools such as
 *  trigger decision, and vertex tools.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#include "OverlayAnalysis/OverlayAnalysis.h"

#include <xAODTracking/VertexContainer.h>
#include <xAODTruth/TruthVertexContainer.h>

#include <TSystem.h>

#include <fstream>
#include <sstream>

/** @brief Default Constructor for OverlayAnalysis.
 */
OverlayAnalysis :: OverlayAnalysis :: OverlayAnalysis () 
  : Analysis ( "OverlayAnalysis" )
{
  m_trigConfigTool   = NULL;
  m_trigDecisionTool = NULL;
}


/** @brief  Constructor for OverlayAnalysis.
 *
 *  @param1 Name of analysis 
 */
OverlayAnalysis :: OverlayAnalysis :: OverlayAnalysis ( const std::string& name  ) 
  : Analysis ( name )
{
  m_trigConfigTool   = NULL;
  m_trigDecisionTool = NULL;
}

/** @brief Destructor for OverlayAnalysis.
 *
 *  Cleans up an OverlayAnalysis object.
 */
OverlayAnalysis :: OverlayAnalysis :: ~OverlayAnalysis()
{
  delete m_trigConfigTool;    
  delete m_trigDecisionTool;   

  m_trigConfigTool   = NULL;
  m_trigDecisionTool = NULL; 
}

/** @brief Setup method for OverlayAnalysis
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode OverlayAnalysis :: OverlayAnalysis :: Setup ()
{
  std::cout << m_analysisName << " Setup" << std::endl; 

  //-----------------
  //  Configs
  //-----------------
  TEnv* config = m_sd->GetConfig();
  
  m_triggerMenu = config->GetValue("triggerMenu","");
  m_v_triggers =
    vectorise( config->GetValue( Form("triggers.%s", m_triggerMenu.c_str() ),"") );

  m_trigConfigTool = NULL;
  m_trigDecisionTool = NULL;
  
  return xAOD::TReturnCode::kSuccess;
}

/** @brief HistInitialize method for analysis.
 *
 *  Initializes the histograms and branches for 
 *  the tree. These will be mostly related to stats from
 *  tools created in initialize(), run stats, etc.
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode OverlayAnalysis :: OverlayAnalysis :: HistInitialize ()
{
  std::cout << m_analysisName << " HistInitialize" << std::endl;

  m_sd->AddOutputToTree<int> ( "eventNumber" , &m_eventNumber  );
  m_sd->AddOutputToTree<int> ( "LBN"         , &m_LBN          );
  m_sd->AddOutputToTree<int> ( "runNumber"   , &m_runNumber    );
  m_sd->AddOutputToTree<bool>( "haveDaqError", &m_haveDaqError );

  m_sd->AddOutputToTree< std::vector<TVector3> >( "vertices", &m_vertices );

  TEnv* config = m_sd->GetConfig();

  bool isData = config->GetValue( "isData", false );

  if( isData ){
    std::cout << "Trigger Menu: " << m_triggerMenu << std::endl; 
    for( auto& tr : m_v_triggers ){
      std::cout << "setting: " << tr << std::endl;
      m_sd->AddOutputToTree<bool> ( Form("passed_%s", tr.c_str()), &m_m_passed_triggers[tr] );
      m_sd->AddOutputToTree<float>( Form("prescale_%s", tr.c_str()), &m_m_prescale_triggers[tr] );
    }
  }

  return xAOD::TReturnCode::kSuccess;
}


/** @brief Initialize method for OverlayAnalysis.
 *
 *  Sets up tools. I.e. Triggers, vertices, etc.
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode OverlayAnalysis :: OverlayAnalysis :: Initialize () 
{
  std::cout << m_analysisName << " Initializing" << std::endl;

  TEnv* config = m_sd->GetConfig();
 
  bool isData = config->GetValue( "isData", false );

  //--------------------------------
  //    Trigger Decision Tool
  //--------------------------------
  if( isData ){
    // Initialize and configure trigger tools
    m_trigConfigTool = new TrigConf::xAODConfigTool("xAODConfigTool"); // gives us access to the meta-data
    CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ),
		  m_trigConfigTool->initialize() );
    
    ToolHandle< TrigConf::ITrigConfigTool > trigConfigHandle( m_trigConfigTool );
    m_trigDecisionTool = new Trig::TrigDecisionTool( "TrigDecisionTool" );
    CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ),
		  m_trigDecisionTool->setProperty( "ConfigTool", trigConfigHandle ) ); // connect the TrigDecisionTool to the ConfigTool
    CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ),
		  m_trigDecisionTool->setProperty( "TrigDecisionKey", "xTrigDecision" ) );
    CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ), 
		  m_trigDecisionTool->initialize() );
  }

  //--------------------------------
  //  Event Statistics Histogram
  //--------------------------------
  m_sd->GetEventStatistics()->GetXaxis()->SetBinLabel( 3, "GRL Reject" );
  m_sd->GetEventStatistics()->GetXaxis()->SetBinLabel( 4, "Trigger Reject" );
  m_sd->GetEventStatistics()->GetXaxis()->SetBinLabel( 5, "Vertex Reject" );
  m_sd->GetEventStatistics()->GetXaxis()->SetBinLabel( 6, "DAQ Reject" );

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Event Loop method for OverlayAnalysis.
 *
 *  @return xAOD::TReturnCode::kSuccess if
 *          everythign worked.
 */
xAOD::TReturnCode OverlayAnalysis :: OverlayAnalysis :: ProcessEvent(){
  xAOD::TEvent* eventStore = m_sd->GetEventStore();

  //---------------------
  // EVENT INFO
  //---------------------
  const xAOD::EventInfo* eventInfo = 0;
  CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ),
		eventStore->retrieve( eventInfo, "EventInfo") );

  // chick if the event is MC or data
  // (many tools are either for MC or data)
  bool isMC = false;
  // check if the even is MC
  if( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true; // can use this later
  }

  m_runNumber   = eventInfo->runNumber();

  m_eventNumber = eventInfo->eventNumber();
  m_LBN         = eventInfo->lumiBlock();
  m_runNumber   = eventInfo->runNumber();

  if( m_sd->DoPrint() ) {
    std::cout << "EventNumber : " << m_eventNumber << "  LBN : " << m_LBN 
	      << "  runNumber : " << m_runNumber   << std::endl; 
  }

  //---------------------
  // Trigger
  //---------------------
  if (!isMC){ // triggers only for Data
    int nPassed = 0;

    // examine the HLT_xe80* chains, see if they passed/failed and their total prescale
    std::vector<const Trig::ChainGroup*> v_chainGroups; 

    for( auto& tr : m_v_triggers ){
      v_chainGroups.push_back( m_trigDecisionTool->getChainGroup( tr ) );
      m_m_passed_triggers[ tr ]   = false;
      m_m_prescale_triggers[ tr ] = 1;
    }
  
    for(auto& chainGroup : v_chainGroups ){
      for(auto& trig : chainGroup->getListOfTriggers()) {
	auto cg = m_trigDecisionTool->getChainGroup(trig);
	std::string thisTrig = trig;
	if( m_sd->DoPrint() ) Info( "execute()", "%30s chain passed(1)/failed(0): %d total chain prescale (L1*HLT): %.1f", thisTrig.c_str(), cg->isPassed(), cg->getPrescale() );
	if( cg->isPassed() ){
	  nPassed++;
	  m_m_passed_triggers[ thisTrig ] = true;
	  m_m_prescale_triggers[ thisTrig ] = cg->getPrescale();
	}
      } // end for loopover chain group matching v_chainGroups entries 
    } // end loop over v_chainGroups
    if( m_sd->DoPrint() ) std::cout << "Event " << m_sd->GetEventCounter() << " passed " << nPassed  << std::endl;
    if( nPassed == 0 ) {
      m_sd->GetEventStatistics()->Fill( "Trigger Reject", 1 );  // trigger reject
    }
  }
 
  //-------------------------------    
  // VERTEX                                                         
  //-------------------------------
  // Clear container from previous event
  m_vertices.clear();
  //Vertex requirement:
  const xAOD::VertexContainer * vertices = 0;
  CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ), 
		eventStore->retrieve( vertices, "PrimaryVertices" ) );
  
  int n_vertices = vertices->size();

  // check if we have at least one vertex
  if( n_vertices < 2 ){ 
    m_sd->GetEventStatistics()->Fill( "Vertex Reject", 1 );  // vertex reject
  }

  // Fill output with vertices
  for( auto& v : *vertices )
    { m_vertices.push_back( TVector3( v->x(), v->y(), v->z() ) ); }
 
  //---------------------
  // DAQ Errors
  //---------------------
  // Assume there are no errors
  m_haveDaqError = false;
  if(!isMC){
    if( 
       (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error ) 
       || (eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error 
	   ) || 
       (eventInfo->errorState(xAOD::EventInfo::SCT)==xAOD::EventInfo::Error ) 
       || (eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) )
      {
	m_sd->GetEventStatistics()->Fill( "DAQ Reject", 1 );  // daq reject
	m_haveDaqError = true;
      }
  }

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Method to finalize OverlayAnalysis
 *
 *  Here you close up any tools / containers needed to close
 *
 *  @return xAOD::TReturnCode
 */
xAOD::TReturnCode OverlayAnalysis :: OverlayAnalysis :: Finalize()
{
  std::cout << m_analysisName << " Finalizing" << std::endl;

  delete m_trigConfigTool;    
  delete m_trigDecisionTool;   

  m_trigConfigTool   = NULL;
  m_trigDecisionTool = NULL;

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Method to finalize histograms / trees
 *
 *  Here you do all final steps for histograms
 *
 *  @return xAOD::TReturnCode
 */
xAOD::TReturnCode OverlayAnalysis :: OverlayAnalysis :: HistFinalize()
{
  std::cout << m_analysisName << " HistFinalize" << std::endl;

  return xAOD::TReturnCode::kSuccess;
}
