/** @file JetAnalysis.cxx
 *  @brief Implementation of JetAnalysis.
 *
 *  Function definitions for JetAnalysis  are provided. 
 *  This class is the main  class for the analysis.
 *  It initializes tools such as GRL,
 *  trigger decision, and vertex tools.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#include "JetAnalysis/JetAnalysis.h"

#include <xAODJet/JetContainer.h>
#include <xAODCore/AuxContainerBase.h>

#include <xAODTruth/TruthEventContainer.h>
#include <xAODTruth/TruthParticleContainer.h>

#include <JetSelectorTools/JetCleaningTool.h>

#include <JetCalibTools/JetCalibrationTool.h>

#include <TMath.h>

/** @brief Default Constructor for Fluctuation Analysis.
 */
JetAnalysis :: JetAnalysis :: JetAnalysis () 
  : Analysis ( "JetAnalysis" )
{
  // tools
  m_jetCleaningTool    = NULL;
  m_jetCalibrationTool = NULL; 
}


/** @brief  Constructor for Fluctuation Analysis.
 *
 *  @param1 Name of analysis 
 */
JetAnalysis :: JetAnalysis :: JetAnalysis ( const std::string& name  ) 
  : YKAnalysis::Analysis ( name )
{
  // tools
  m_jetCleaningTool    = NULL;
  m_jetCalibrationTool = NULL; 
}

/** @brief Destructor for Fluctuation Analysis.
 *
 *  Cleans up an JetAnalysis object.
 */
JetAnalysis :: JetAnalysis :: ~JetAnalysis()
{
  // tools
  delete m_jetCleaningTool;
  delete m_jetCalibrationTool;
  m_jetCleaningTool    = NULL;
  m_jetCalibrationTool = NULL;  
}

/** @brief Setup method for Fluctuation Analysis
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: Setup ()
{
  std::cout << m_analysisName << " Setup" << std::endl; 

  //-----------------
  //  Configs
  //-----------------
  TEnv* config = m_sd->GetConfig();

  m_isData            = config->GetValue( "isData", false );

  m_recoJetAlgorithm  = config->GetValue( "recoJetAlgorithm",  "" );
  m_recoJetContainer  = config->GetValue( "recoJetContainer",  "" );
  m_truthJetContainer = config->GetValue( "truthJetContainer", "" );
  m_calibConfig       = config->GetValue( "calibConfig",       "" );
  m_calibSequence     = config->GetValue( "calibSequence",     "" );
 
  m_jetPtMin          = config->GetValue( "jetPtMin", 20 ) * 1000; // GeV
  m_jetRparameter     = config->GetValue( "jetRparameter", 0.4 ) ; 

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
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: HistInitialize ()
{
  std::cout << m_analysisName << " HistInitialize" << std::endl;
  
  if( !m_isData ){
    m_sd->AddOutputToTree< std::vector<TLorentzVector> >("vT_jets" , &vT_jets);
  }

  m_sd->AddOutputToTree< std::vector<TLorentzVector> >("vR_C_jets"  , &vR_C_jets );
  m_sd->AddOutputToTree< std::vector<TLorentzVector> >("vR_DF_jets" , &vR_DF_jets);
  m_sd->AddOutputToTree< std::vector<TLorentzVector> >("vR_EM_jets" , &vR_EM_jets);
  
  m_sd->AddOutputToTree< std::vector<bool> >("v_isCleanJet", &v_isCleanJet );
  
  return xAOD::TReturnCode::kSuccess;
}


/** @brief Initialize method for JetAnalysis.
 *
 *  Sets up tools. I.e. GRL, 
 *  Triggers, vertices, etc. Basically, things
 *  common to most analysis.
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: Initialize () 
{
  std::cout << m_analysisName << " Initializing" << std::endl;

  // Jet Cleaning
  m_jetCleaningTool = new JetCleaningTool("JetCleaning");
  m_jetCleaningTool->msg().setLevel( MSG::DEBUG ); 
  CHECK_STATUS( Form("%s::initialize",m_analysisName.c_str() ),
		m_jetCleaningTool->setProperty( "CutLevel", "LooseBad"));
  CHECK_STATUS( Form("%s::initialize",m_analysisName.c_str() ),
	       m_jetCleaningTool->setProperty("DoUgly", false));
  CHECK_STATUS( Form("%s::initialize",m_analysisName.c_str() ),
	       m_jetCleaningTool->initialize());

  // Jet Calibration
  const std::string name = "JetAnalysis"; //string describing the current thread, for logging
  std::string jetAlgorithm   =  m_recoJetAlgorithm; //String describing your jet collection, 
  std::string config         =  m_calibConfig;      //Path to global config used to initialize the tool
  std::string calibSeq       =  m_calibSequence;    //String describing the calibration sequence to apply
  bool isData                =  m_isData;            //bool describing if the events are data or from simulation

  m_jetCalibrationTool = new JetCalibrationTool(name, jetAlgorithm, config, calibSeq, isData);
  m_jetCalibrationTool->msg().setLevel( MSG::DEBUG ); 
  // Initialize the tool
  CHECK_STATUS( Form("%s::initialize",m_analysisName.c_str() ),
		m_jetCalibrationTool->initializeTool(name) );


  // insitu calibration
  TString jetAlgo_insitu = "AntiKt4EMTopo"; //String describing your calibration
  TString config_insitu = 
    "JES_2015dataset_recommendation_Feb2016.config"; //Path to global config 
  const std::string name_insitu = "insitu"; //string describing the current thread
  TString calibSeq_insitu = "Insitu_DEV"  ; //String describing the calib sequence 
  
  
  m_jetCalibration_insitu = 
    new JetCalibrationTool(name_insitu, jetAlgo_insitu, 
			   config_insitu, calibSeq_insitu, true);
  // initialize the tool
  CHECK_STATUS( Form("%s::initialize",m_analysisName.c_str() ),
		m_jetCalibration_insitu->initializeTool(name_insitu));

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Event Loop method for Fluctuation Analysis.
 *
 *  @return xAOD::TReturnCode::kSuccess if
 *          everythign worked.
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: ProcessEvent(){
  xAOD::TEvent* eventStore = m_sd->GetEventStore();

  //---------------------
  // EVENT INFO
  //---------------------
  const xAOD::EventInfo* eventInfo = 0;
  CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ),
		eventStore->retrieve( eventInfo, "EventInfo") );

  // check if the event is MC or data
  // (many tools are either for MC or data)
  bool isMC = false;
  // check if the even is MC
  if( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true; // can use this later
  }

  bool keep = true;

  //DAQ errors
  if(!isMC){
    if( 
       (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error ) 
       || (eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error 
	   ) || 
       (eventInfo->errorState(xAOD::EventInfo::SCT)==xAOD::EventInfo::Error ) 
       || (eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) )
      {
	return xAOD::TReturnCode::kSuccess;
      }
  }

  //-------------------------------    
  // JETS                                                            
  //-------------------------------  
  // clear containers from previous event
  vT_jets.clear() ; 

  vR_C_jets.clear() ;
  vR_EM_jets.clear() ; 
  vR_DF_jets.clear() ; 

  v_isCleanJet.clear() ;

  // jet containers, initialize here to avoid problems later
  const xAOD::JetContainer* recoJets  = 0;
  const xAOD::JetContainer* truthJets = 0;

  // get the reconstructed containter (jets)
  CHECK_STATUS(  Form("%s::execute",m_analysisName.c_str() ),
		 eventStore->retrieve( recoJets, m_recoJetContainer.c_str() ) );

  if( m_sd->DoPrint() ) 
    printf("%s  :  %i  \n", m_recoJetContainer.c_str(), (int)recoJets->size() );
  
  int numGoodJets = 0;

  // Create the new container and its auxiliary store.
  xAOD::JetContainer*     calibRecoJets    = new xAOD::JetContainer();
  xAOD::AuxContainerBase* calibRecoJetsAux = new xAOD::AuxContainerBase();
  calibRecoJets->setStore( calibRecoJetsAux ); //< Connect the two

  std::vector<bool> vTemp_isCleanJet;

  for( const auto* jet : *recoJets ){
  
    if( m_jetCleaningTool->accept( *jet ) ) vTemp_isCleanJet.push_back(true);
    else vTemp_isCleanJet.push_back(false);
    numGoodJets++;
    
    xAOD::Jet* newjet = new xAOD::Jet();
    newjet->makePrivateStore( *jet );

    const xAOD::JetFourMom_t pileupscale_jetP4 = newjet->jetP4("JetEMScaleMomentum");
    newjet->setJetP4( "JetPileupScaleMomentum", pileupscale_jetP4 );

    calibRecoJets->push_back( newjet );
    
    CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ),
		  m_jetCalibrationTool->applyCalibration( *newjet ) ); 

    //Insitu corrections, need to be done at GSW scale
    const xAOD::JetFourMom_t jet_4mom_xcalib = newjet->jetP4();
    newjet->setJetP4("JetGSCScaleMomentum", jet_4mom_xcalib);
    
    if( !isMC ){ CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ), 
			       m_jetCalibration_insitu->applyCalibration( *newjet ) );
    }  
  } // end for loop over jets
 
  if( m_sd->DoPrint() )
    printf("\nNum Good %s Jets : %i\n", m_recoJetContainer.c_str(), numGoodJets);

  if( isMC ){ // get the truth containter (jets)
    CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ), 
		  eventStore->retrieve( truthJets, m_truthJetContainer.c_str() ) );
    
    if( m_sd->DoPrint() )
      printf("\n%s  :  %i \n", m_truthJetContainer.c_str(), (int)truthJets->size() );
  }

  //-------------------------------
  
  // MC
  // Save Truth and Reco
  if( isMC ){
    SaveJets( m_jetPtMin, recoJets, calibRecoJets, vTemp_isCleanJet,
	      vR_DF_jets, vR_EM_jets, vR_C_jets, v_isCleanJet );
    SaveJets( m_jetPtMin, truthJets, vT_jets ); 
  } 			
  // DATA
  // no pairing for data since no truth jets just save them
  else if( !isMC ){
    SaveJets( m_jetPtMin, recoJets, calibRecoJets, vTemp_isCleanJet,
	      vR_DF_jets, vR_EM_jets, vR_C_jets, v_isCleanJet );
  } 
    
  // delete the deep copies
  delete calibRecoJets;
  delete calibRecoJetsAux;
  calibRecoJets    = NULL;
  calibRecoJetsAux = NULL;  
  
  return xAOD::TReturnCode::kSuccess;
}

/** @brief Method to finalize Fluctuation Analysis
 *
 *  Here you close up any tools / containers needed to close
 *
 *  @return xAOD::TReturnCode
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: Finalize()
{
  std::cout << m_analysisName << " Finalizing" << std::endl;

  // tools
  delete m_jetCleaningTool;
  delete m_jetCalibrationTool;
  m_jetCleaningTool    = NULL;
  m_jetCalibrationTool = NULL; 

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Method to finalize histograms / trees
 *
 *  Here you do all final steps for histograms
 *
 *  @return xAOD::TReturnCode
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: HistFinalize()
{
  std::cout << m_analysisName << " HistFinalize" << std::endl;

  return xAOD::TReturnCode::kSuccess;
}



// calculate deltaR = sqrt( deltaphi^2 + deltaeta^2)
Float_t JetAnalysis :: JetAnalysis :: DeltaR( const xAOD::Jet* jet1 , 
					      const xAOD::Jet* jet2 )
{  
  Float_t deltaEta = jet1->eta() - jet2->eta();
  Float_t deltaPhi = TMath::Abs( jet1->phi() - jet2->phi() );
  if(deltaPhi > TMath::Pi())
    deltaPhi = 2*TMath::Pi() - deltaPhi;
  return TMath::Sqrt( deltaEta*deltaEta + deltaPhi*deltaPhi );
}


// save just the reco jets
void JetAnalysis :: JetAnalysis :: SaveJets(  float pTmin,  
					      const xAOD::JetContainer * recoJets,
					      const xAOD::JetContainer * calibRecoJets,	
					      std::vector<bool>&           vTemp_isCleanJet,
					      std::vector<TLorentzVector>& vR_DF_jets,
					      std::vector<TLorentzVector>& vR_EM_jets,
					      std::vector<TLorentzVector>& vR_C_jets ,
					      std::vector<bool>&           v_isCleanJet  ){
  
  int nSavedJets = 0;

  for( unsigned int iJet = 0; iJet < recoJets->size(); iJet++){ 
    const xAOD::JetFourMom_t calibJetP4 = calibRecoJets->at(iJet)->jetP4();
    
    if( calibJetP4.pt() < pTmin && nSavedJets >= 3 ) continue;
  
    const xAOD::JetFourMom_t defaultJetP4 = recoJets->at(iJet)->jetP4();
    const xAOD::JetFourMom_t emScaleJetP4 = recoJets->at(iJet)->jetP4("JetEMScaleMomentum");
  
    vR_DF_jets.push_back( TLorentzVector( defaultJetP4.px(), defaultJetP4.py(), 
					  defaultJetP4.pz(), defaultJetP4.e() ) );  

    vR_EM_jets.push_back( TLorentzVector( emScaleJetP4.px(), emScaleJetP4.py(), 
					  emScaleJetP4.pz(), emScaleJetP4.e() ) );  

    vR_C_jets.push_back ( TLorentzVector( calibJetP4.px(), calibJetP4.py(), 
					  calibJetP4.pz(), calibJetP4.e() ) );

    v_isCleanJet.push_back( vTemp_isCleanJet.at(iJet) );
  
    nSavedJets++;
  }
  
}


// save the jets
void JetAnalysis :: JetAnalysis :: SaveJets(  float pTmin,  
					      const xAOD::JetContainer * jets , 
					      std::vector<TLorentzVector>& v_jets  ){
  int nSavedJets = 0;
  
  for( const auto* jet : *jets ){
    const xAOD::JetFourMom_t jetP4 = jet->jetP4();

    if( jetP4.pt() < pTmin && nSavedJets >= 3 ) continue;

    v_jets.push_back( TLorentzVector( jetP4.px(), jetP4.py(), 
				      jetP4.pz(), jetP4.e() ) ); 
  
    nSavedJets++;
  }
}


// save the jets
void JetAnalysis :: JetAnalysis :: SaveJets(  float pTmin,  
					      const xAOD::JetContainer * jets , 
					      std::vector<TLorentzVector>& v_jets ,
					      const std::string& scale ){
  int nSavedJets = 0;

  for( const auto* jet : *jets ){
    const xAOD::JetFourMom_t jetP4 = jet->jetP4( scale );

    if( jetP4.pt() < pTmin && nSavedJets >= 3 ) continue;

    v_jets.push_back( TLorentzVector( jetP4.px(), jetP4.py(), jetP4.pz(), jetP4.e() ) );

    nSavedJets++;
  } 
}
