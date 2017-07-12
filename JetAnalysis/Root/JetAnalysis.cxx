/** @file JetAnalysis.cxx
 *  @brief Implementation of JetAnalysis.
 *
 *  Function definitions for JetAnalysis  are provided. 
 *  This class is the main  class for the jet analysis.
 *  Does some jet selection, calibration. 
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
#include <JetUncertainties/JetUncertaintiesTool.h>
#include <JetAnalysis/HIJESUncertaintyProvider.h>

#include <TMath.h>

/** @brief Default Constructor for Fluctuation Analysis.
 */
JetAnalysis :: JetAnalysis :: JetAnalysis () 
  : JetAnalysis ( "JetAnalysis" )
{}


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
  m_jetUncertaintyTool   = NULL;
  m_hiJetUncertaintyTool = NULL; 
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
  delete m_jetUncertaintyTool;
  delete m_hiJetUncertaintyTool;
  m_jetCleaningTool      = NULL;
  m_jetCalibrationTool   = NULL;
  m_jetUncertaintyTool   = NULL;
  m_hiJetUncertaintyTool = NULL; 
}

/** @brief Setup method for Jet Analysis
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

  m_isData            = config->GetValue( "isData"       , false );
  m_doSystematics     = config->GetValue( "doSystematics", false );

  m_recoJetAlgorithm  = config->GetValue( "recoJetAlgorithm" , "" );
  m_recoJetContainer  = config->GetValue( "recoJetContainer" , "" );
  m_truthJetContainer = config->GetValue( "truthJetContainer", "" );
  m_trigJetContainer  = config->GetValue( "trigJetContainer" , "" );

  m_calibConfig       = config->GetValue( "calibConfig"  , "JES_MC15c_HI_Nov2016.config" );
  m_calibSequence     = config->GetValue( "calibSequence", "EtaJES_Insitu" );
 
  m_jetPtMin          = config->GetValue( "jetPtMin", 10 ) * 1000; // MeV
  m_jetRparameter     = config->GetValue( "jetRparameter", 0.4 ) ; 

  m_nSysUncert        = config->GetValue( "nSystematics"   , 19 );
  m_nSysUncert_pp     = config->GetValue( "nSystematics_pp", 17 );
  m_nSysUncert_HI     = config->GetValue( "nSystematics_HI", 2  );

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

  // Reco jets  
  m_sd->AddOutputToTree< std::vector<TLorentzVector> >
    ("vR_C_jets"  , &vR_C_jets );

  // Truth jets. Only in MC
  if( !m_isData )
    {m_sd->AddOutputToTree< std::vector<TLorentzVector> >
	("vT_jets" , &vT_jets); }

  m_sd->AddOutputToTree< std::vector<bool> >
    ("v_isCleanJet", &v_isCleanJet );

  // Trigger jets. Only in Data
  if( m_isData )
    { m_sd->AddOutputToTree< std::vector<TLorentzVector> >
	("vTrig_jets" , &vTrig_jets); }

  // add uncertainty unc
  if( !m_isData )
    { m_sd->AddOutputToTree< std::vector<std::vector<float> > >
	("v_sysUncert", &v_sysUncert); }

  return xAOD::TReturnCode::kSuccess;
}


/** @brief Initialize method for JetAnalysis.
 *
 *  Sets up tools. I.e. Calibration
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: Initialize () 
{
  std::cout << m_analysisName << " Initializing" << std::endl;

  const char* statusL = Form("%s::initialize",m_analysisName.c_str() ); 

  // ----- Jet Cleaning
  m_jetCleaningTool = new JetCleaningTool("JetCleaning");
  m_jetCleaningTool->msg().setLevel( MSG::DEBUG ); 
  CHECK_STATUS( statusL, m_jetCleaningTool->setProperty( "CutLevel", "LooseBad"));
  CHECK_STATUS( statusL, m_jetCleaningTool->setProperty("DoUgly", false));
  CHECK_STATUS( statusL, m_jetCleaningTool->initialize() );

  // ----- Jet Calibration
  const std::string name    = "JetAnalysis";       //string describing the current thread, for logging
  std::string jetAlgorithm  =  m_recoJetAlgorithm; //String describing your jet collection, 
  std::string config        =  m_calibConfig;      //Path to global config used to initialize the tool
  std::string calibSeq      =  m_calibSequence;    //String describing the calibration sequence to apply
  bool isData               =  m_isData;           //bool describing if the events are data or from simulation

  // Call constructor
  m_jetCalibrationTool = new JetCalibrationTool(name);
  // Set properties
  CHECK_STATUS( statusL, m_jetCalibrationTool->setProperty( "JetCollection", jetAlgorithm ) );
  CHECK_STATUS( statusL, m_jetCalibrationTool->setProperty( "ConfigFile", config ) );
  CHECK_STATUS( statusL, m_jetCalibrationTool->setProperty( "CalibSequence", calibSeq ) );
  CHECK_STATUS( statusL, m_jetCalibrationTool->setProperty( "IsData", isData ) );
  m_jetCalibrationTool->msg().setLevel( MSG::DEBUG ); 
  // Initialize the tool
  CHECK_STATUS( statusL, m_jetCalibrationTool->initializeTool(name) );

  // ----- JES (pp)
  // Call Constructor
  m_jetUncertaintyTool = new JetUncertaintiesTool();
  CHECK_STATUS( statusL, m_jetUncertaintyTool->setProperty("JetDefinition","AntiKt4EMTopo") );
  CHECK_STATUS( statusL, m_jetUncertaintyTool->setProperty("MCType","MC15") );
  CHECK_STATUS( statusL, m_jetUncertaintyTool->setProperty
		("ConfigFile","JES_2015/ICHEP2016/JES2015_19NP.config") );
  // Initialize the tool
  CHECK_STATUS( statusL, m_jetUncertaintyTool->initialize() );

  // ----- JES (HI)
  // Call Constructor
  m_hiJetUncertaintyTool = new HIJESUncertaintyProvider("HIJESUncert_data15_5TeV.root");
  m_hiJetUncertaintyTool->UseJESTool(true);
  m_hiJetUncertaintyTool->UseGeV(false);

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Event Loop method for Fluctuation Analysis.
 *
 *  @return xAOD::TReturnCode::kSuccess if
 *          everythign worked.
 */
xAOD::TReturnCode JetAnalysis :: JetAnalysis :: ProcessEvent(){
  xAOD::TEvent* eventStore = m_sd->GetEventStore();

  const char* statusL = Form("%s::execute",m_analysisName.c_str() );
  //---------------------
  // EVENT INFO
  //---------------------
  const xAOD::EventInfo* eventInfo = 0;
  CHECK_STATUS( statusL, eventStore->retrieve( eventInfo, "EventInfo") );

  // check if the event is MC or data
  // (many tools are either for MC or data)
  bool isMC = false;
  // check if the even is MC
  if( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true; // can use this later
  }

  bool isData = isMC ? false : true;

  //-------------------------------    
  // JETS                                                            
  //-------------------------------  
  // clear containers from previous event
  vR_C_jets   .clear();
  vT_jets     .clear(); 
  v_sysUncert .clear();
  v_isCleanJet.clear();

  // jet containers, initialize here to avoid problems later
  const xAOD::JetContainer* recoJets  = 0;
  const xAOD::JetContainer* truthJets = 0;

  // get the reconstructed containter (jets)
  CHECK_STATUS( statusL, eventStore->retrieve( recoJets, m_recoJetContainer.c_str() ) );

  if( m_sd->DoPrint() ) 
    printf("%s  :  %i", m_recoJetContainer.c_str(), (int)recoJets->size() );
  
  // Create the new container and its auxiliary store.
  xAOD::JetContainer*     calibRecoJets    = new xAOD::JetContainer();
  xAOD::AuxContainerBase* calibRecoJetsAux = new xAOD::AuxContainerBase();
  calibRecoJets->setStore( calibRecoJetsAux ); //< Connect the two

  for( const auto& jet : *recoJets ){
    bool isCleanJet = m_jetCleaningTool->accept( *jet );
    
    xAOD::Jet* newJet = new xAOD::Jet();
    newJet->makePrivateStore( *jet );

    const xAOD::JetFourMom_t pileupscale_jetP4 = newJet->jetP4("JetEMScaleMomentum");
    newJet->setJetP4( "JetPileupScaleMomentum", pileupscale_jetP4 );
    
    CHECK_STATUS( statusL, m_jetCalibrationTool->applyCalibration( *newJet ) ); 

    // if the calibrated pT is less than a cut, dont
    // save or do anything else with this jet 
    if( newJet->jetP4().pt() < m_jetPtMin ){ continue; }

    calibRecoJets->push_back( newJet );
    v_isCleanJet.push_back( isCleanJet );

    // do systematic uncertainties
    if( isMC && m_doSystematics ){
      std::vector< float > jet_sys_uncert;
      jet_sys_uncert.reserve( m_nSysUncert );
      UncertaintyProviderJES( newJet, jet_sys_uncert );
      v_sysUncert.push_back( jet_sys_uncert );
    }
  }// end for loop over jets
 
  // get the truth containter (jets)
  if( isMC ){
    CHECK_STATUS
      ( statusL, eventStore->retrieve( truthJets, m_truthJetContainer.c_str() ) );
    
    if( m_sd->DoPrint() )
      { printf("\n%s  :  %i", m_truthJetContainer.c_str(), (int)truthJets->size()); }
  }

  //-------------------------------
  
  // MC
  // Save Truth and Reco
  if( isMC ){
    SaveJets( calibRecoJets, vR_C_jets );
    SaveJets( truthJets, vT_jets, m_jetPtMin ); 
  } 			
  // DATA
  // no pairing for data since no truth jets just save them
  else if( isData ){
    SaveJets( calibRecoJets, vR_C_jets );
  } 
  
  // delete the deep copies
  delete calibRecoJets;
  delete calibRecoJetsAux;
  calibRecoJets    = NULL;
  calibRecoJetsAux = NULL;  
  
  //-------------------------------    
  // TRIGGER JETS                                                            
  //-------------------------------  
  // clear containers from previous event
  vTrig_jets.clear() ; 

  // jet containers, initialize here to avoid problems later
  const xAOD::JetContainer* trigJets  = 0;

  if( isData ){
    // get the trigger jets containter (jets)
    CHECK_STATUS
      (  statusL, eventStore->retrieve( trigJets, m_trigJetContainer.c_str() ) );

    if( m_sd->DoPrint() ) 
      printf("%s  :  %i", m_recoJetContainer.c_str(), (int)recoJets->size() );
   
    SaveJets( trigJets, vTrig_jets, m_jetPtMin ); 
  }

  /*
  if( m_sd->DoPrint() ){
    std::cout << "\nHave " << vR_C_jets.size() << " calibrated Jets" << std::endl;
    std::cout <<   "Have " << vT_jets.size()   << " truth Jets" << std::endl;
    int counter = 0;
    for( auto & v : v_sysUncert ){
      std::cout << "jet " << counter++ << " : " << std::endl; 
      int sys = 0;
      for( auto & s : v ){
	std::cout << sys++ << " > " << s << std::endl;
      }
    }
  }
  */

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
  delete m_jetUncertaintyTool;
  delete m_hiJetUncertaintyTool;
  m_jetCleaningTool      = NULL;
  m_jetCalibrationTool   = NULL; 
  m_jetUncertaintyTool   = NULL;
  m_hiJetUncertaintyTool = NULL;

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

void JetAnalysis :: JetAnalysis :: UncertaintyProviderJES
( const xAOD::Jet* jet,
  std::vector<float>& v_uncert ){

  for( int component = 0; component < m_nSysUncert; component++ ){
    double jetPt  = jet->pt();
    double jetEta = jet->eta();
    
    float uncertainty = 0;
    // if a pp factor, just use this, and continue to next
    if( component < m_nSysUncert_pp ){ 
      uncertainty = m_jetUncertaintyTool->getUncertainty( component,(*jet) );
      v_uncert.push_back( uncertainty );
      continue; 
    }

    // if not a standard pp uncertainty, do more 
    float HIJESuncertainty = 0;
    if ( component == m_nSysUncert_pp ){
      HIJESuncertainty = 
	sqrt(pow( m_hiJetUncertaintyTool->GetUncertaintyComponent
		 ("flav_composition",jetPt, jetEta),2) + 
	     pow( m_hiJetUncertaintyTool->GetUncertaintyComponent
		 ("flav_response",jetPt, jetEta),2));
    } else if ( component == m_nSysUncert_pp + 1 ) {
      /*
	HIJESuncertainty = m_hiJetUncertaintyTool->
	GetHIJESHisto( jetEta )->Interpolate( jetPt/1000.);
      */
      // for now, the above doesnt work. histograms are bad
      HIJESuncertainty = 0;
    }
   
    v_uncert.push_back( HIJESuncertainty );
  }
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

// save the jets
void JetAnalysis :: JetAnalysis :: SaveJets(  const xAOD::JetContainer* jets , 
					      std::vector<TLorentzVector>& v_jets,
					      float pTmin ){
  for( const auto& jet : *jets ){
    const xAOD::JetFourMom_t jetP4 = jet->jetP4();
    if( jetP4.pt() < pTmin ) continue;
    v_jets.push_back( TLorentzVector( jetP4.px(), jetP4.py(), jetP4.pz(), jetP4.e() ) ); 
  }
}


// save the jets
void JetAnalysis :: JetAnalysis :: SaveJets( const xAOD::JetContainer* jets , 
					     std::vector<TLorentzVector>& v_jets ,
					     const std::string& scale,
					     float pTmin ){
  for( const auto& jet : *jets ){
    const xAOD::JetFourMom_t jetP4 = jet->jetP4( scale );
    if( jetP4.pt() < pTmin ) continue;
    v_jets.push_back( TLorentzVector( jetP4.px(), jetP4.py(), jetP4.pz(), jetP4.e() ) );
  } 
}
