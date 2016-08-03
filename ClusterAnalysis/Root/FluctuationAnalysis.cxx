/** @file FluctuationAnalysis.cxx
 *  @brief Implementation of FluctuationAnalysis.
 *
 *  Function definitions for FluctuationAnalysis  are provided. 
 *  This class is the main  class for the analysis.
 *  It initializes tools such as GRL,
 *  trigger decision, and vertex tools.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#include "ClusterAnalysis/FluctuationAnalysis.h"

#include <xAODCaloEvent/CaloClusterContainer.h>

#include <xAODHIEvent/HIEventShapeContainer.h>

/** @brief Default Constructor for Fluctuation Analysis.
 */
ClusterAnalysis :: FluctuationAnalysis :: FluctuationAnalysis () 
  : Analysis ( "FluctuationAnalysis" )
{
  m_window_Eta_size = 7;
  m_window_Phi_size = 7;

  m_nEtaBins = 100;
  m_etaMin = -5;             m_etaMax = 5;      
  
  m_nPhiBins = 64;
  m_phiMin = -TMath::Pi();   m_phiMax = TMath::Pi(); 

  m_nFCalEtBins = 500 ; 
  m_fCalEtMin = 0;           m_fCalEtMax = 10 ;  // TeV
}


/** @brief  Constructor for Fluctuation Analysis.
 *
 *  @param1 Name of analysis 
 */
ClusterAnalysis :: FluctuationAnalysis :: FluctuationAnalysis ( const std::string& name  ) 
  : YKAnalysis::Analysis ( name )
{
  m_window_Eta_size = 7;
  m_window_Phi_size = 7;

  m_nEtaBins = 100;
  m_etaMin = -5;             m_etaMax = 5;      
  
  m_nPhiBins = 64;
  m_phiMin = -TMath::Pi();   m_phiMax = TMath::Pi(); 

  m_nFCalEtBins = 500 ; 
  m_fCalEtMin = 0;           m_fCalEtMax = 10 ;  // TeV

}

/** @brief Destructor for Fluctuation Analysis.
 *
 *  Cleans up an FluctuationAnalysis object.
 */
ClusterAnalysis :: FluctuationAnalysis :: ~FluctuationAnalysis()
{}

/** @brief Setup method for Fluctuation Analysis
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode ClusterAnalysis :: FluctuationAnalysis :: Setup ()
{
  std::cout << m_analysisName << " Setup" << std::endl; 

  //-----------------
  //  Configs
  //-----------------
  TEnv* config = m_sd->GetConfig();

  m_window_Eta_size = config->GetValue( "fluctuationWindowEtaSize", 7);
  m_window_Phi_size = config->GetValue( "fluctuationWindowPhiSize", 7);

  m_clusterContainerName = config->GetValue( "clusterContainerName" , "" ); 

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
xAOD::TReturnCode ClusterAnalysis :: FluctuationAnalysis :: HistInitialize ()
{
  std::cout << m_analysisName << " HistInitialize" << std::endl;

  double etWindowMax = 500; int etWindowBins = 500;
  if( m_window_Eta_size == 11 ){
    etWindowMax  = 500;
    etWindowBins = 500;
  }
  else if( m_window_Eta_size == 7){
    etWindowMax  = 250;
    etWindowBins = 250;
  }
  else if( m_window_Eta_size == 5){
    etWindowMax  = 125;
    etWindowBins = 125;
  }
  else if( m_window_Eta_size == 3){
    etWindowMax  = 75;
    etWindowBins = 150;
  }
  else if( m_window_Eta_size ){
    etWindowMax  = 25;
    etWindowBins = 50;
  }

  std::vector< double > FCalEtRanges;
  FCalEtRanges.push_back(0.00);
  FCalEtRanges.push_back(0.02);
  FCalEtRanges.push_back(0.22);
  FCalEtRanges.push_back(0.56);
  FCalEtRanges.push_back(1.22);
  FCalEtRanges.push_back(2.38);
  FCalEtRanges.push_back(15.0);

  TAxis* axis = 0;
  int nFCalEtBins = FCalEtRanges.size() - 1;

  m_sd->AddOutputToTree< double >( "FCalEt", &m_FCalEt );
  m_sd->AddOutputToTree< double >( "caloFluctuation", &m_caloFluctuation );
  m_sd->AddOutputToTree< std::vector< double > >( "v_caloFluctuationEtaSlices", &m_v_caloFluctuationEtaSlices );

  // "Normal"
  h3_EtaPhiFCalEt = new TH3D("h3_EtaPhiFCalEt","h3_EtaPhiFCalEt;#eta;#phi;#SigmaE_{T} (3.2<|#eta|<4.6) [TeV]",
			     m_nEtaBins, m_etaMin, m_etaMax,
			     m_nPhiBins, m_phiMin, m_phiMax,
			     nFCalEtBins, FCalEtRanges.front(), FCalEtRanges.back() );     
  axis = h3_EtaPhiFCalEt->GetZaxis();
  axis->Set( nFCalEtBins, &( FCalEtRanges[0]) );  
  h3_EtaPhiFCalEt->Sumw2();
  m_sd->AddOutputHistogram( h3_EtaPhiFCalEt );
 
  // UW
  h3_EtaPhiFCalEt_UW = new TH3D("h3_EtaPhiFCalEt_UW","h3_EtaPhiFCalEt;#eta;#phi;#SigmaE_{T} (3.2<|#eta|<4.6) [TeV]",
				m_nEtaBins, m_etaMin, m_etaMax,
				m_nPhiBins, m_phiMin, m_phiMax,
				nFCalEtBins, FCalEtRanges.front(), FCalEtRanges.back() );
  axis = h3_EtaPhiFCalEt_UW->GetZaxis();
  axis->Set( nFCalEtBins, &( FCalEtRanges[0]) );  
  m_sd->AddOutputHistogram( h3_EtaPhiFCalEt_UW );

  // FCalEt
  h1_FCalEt  = new TH1D("h1_FCalEt", ";#SigmaE_{T} (3.2<|#eta|<4.6) [TeV];Entries", m_nFCalEtBins, m_fCalEtMin, m_fCalEtMax);
  m_sd->AddOutputHistogram( h1_FCalEt );

  return xAOD::TReturnCode::kSuccess;
}


/** @brief Initialize method for FluctuationAnalysis.
 *
 *  Sets up tools. I.e. GRL, 
 *  Triggers, vertices, etc. Basically, things
 *  common to most analysis.
 *
 *  @return xAOD::TReturnCode 
 */
xAOD::TReturnCode ClusterAnalysis :: FluctuationAnalysis :: Initialize () 
{
  std::cout << m_analysisName << " Initializing" << std::endl;

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Event Loop method for Fluctuation Analysis.
 *
 *  @return xAOD::TReturnCode::kSuccess if
 *          everythign worked.
 */
xAOD::TReturnCode ClusterAnalysis :: FluctuationAnalysis :: ProcessEvent(){
  xAOD::TEvent* eventStore = m_sd->GetEventStore();

  //-------------------------------    
  // FCALSUM                                                              
  //-------------------------------
  m_FCalEt = -1; 

  // calosums container, initialize here to avoid problems later
  const xAOD::HIEventShapeContainer* caloSumContainer = 0;    // calosum container    

  CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ), 
		eventStore->retrieve( caloSumContainer, "CaloSums" ) );

  int x = 0;
  // loop over calosums
  for( const auto* caloSum : *caloSumContainer ){
    if (x == 5) {
      m_FCalEt = (  caloSum->et() * 0.001 * 0.001 ); // TeV !!!
    }
    x++;
  }
  h1_FCalEt->Fill( m_FCalEt );

  //-------------------------------    
  // CALO CLUSTERS                                                                
  //-------------------------------   
  // calocluster container, initialize here to avoid problems later
  const xAOD::CaloClusterContainer* caloClusterContainer = 0;
  
  // Get CaloCalTopoCluster container (calocluster)                
  CHECK_STATUS( Form("%s::execute",m_analysisName.c_str() ), 
		eventStore->retrieve( caloClusterContainer, m_clusterContainerName.c_str() ) );
      
  // make a (temp) 2d histo which has eta,phi distribution of Et.
  // it goes to the tool
  TH2F h2_EtaPhi("h2_EtaPhi","h2_EtaPhi",  m_nEtaBins, m_etaMin, m_etaMax,  m_nPhiBins, m_phiMin, m_phiMax);

  // loop over cluster container
  for(const auto* caloCluster : *caloClusterContainer){
    
    /*
    double cc_Eta_cal  = caloCluster->calEta();            // Eta  
    double cc_Phi_cal  = caloCluster->calPhi();            // Phi  
    double cc_E_cal    = caloCluster->calE()  * 0.001;     // E  in GeV
    double cc_Et_cal   = cc_E_cal / TMath::CosH( cc_Eta_cal ); // Et in GeV

    double cc_Eta_raw  = caloCluster->rawEta();            // Eta
    double cc_Phi_raw  = caloCluster->rawPhi();            // Phi
    double cc_E_raw    = caloCluster->rawE()  * 0.001;     // E  in GeV
    double cc_Et_raw   = cc_E_raw / TMath::CosH( cc_Eta_raw ); // Et in GeV

    double cc_Eta_alt  = caloCluster->altEta();            // Eta
    double cc_Phi_alt  = caloCluster->altPhi();            // Phi
    double cc_E_alt    = caloCluster->altE()  * 0.001;     // E  in GeV
    double cc_Et_alt   = cc_E_alt / TMath::CosH( cc_Eta_alt ); // Et in GeV
    */

    double cc_Eta  = caloCluster->eta();              // Phi
    double cc_Phi  = caloCluster->phi();              // Phi
    double cc_E    = caloCluster->e() * 0.001;        // E in GeV
    double cc_Et   = cc_E / TMath::CosH( cc_Eta );    // Et in GeV

    h3_EtaPhiFCalEt->Fill( cc_Eta, cc_Phi, m_FCalEt, cc_Et);
    h3_EtaPhiFCalEt_UW->Fill( cc_Eta, cc_Phi, m_FCalEt, 1);

    h2_EtaPhi.Fill( cc_Eta, cc_Phi, cc_Et );  // temp histo to be sent to AnalyzeFluctiations
  } // end for loop over cluster
  
  m_v_caloFluctuationEtaSlices.clear();

  m_caloFluctuation = AnalyzeFluctuations( &h2_EtaPhi, 
					   4.9,
					   m_v_caloFluctuationEtaSlices );
  
  return xAOD::TReturnCode::kSuccess;
}

/** @brief Method to finalize Fluctuation Analysis
 *
 *  Here you close up any tools / containers needed to close
 *
 *  @return xAOD::TReturnCode
 */
xAOD::TReturnCode ClusterAnalysis :: FluctuationAnalysis :: Finalize()
{
  std::cout << m_analysisName << " Finalizing" << std::endl;

  return xAOD::TReturnCode::kSuccess;
}

/** @brief Method to finalize histograms / trees
 *
 *  Here you do all final steps for histograms
 *
 *  @return xAOD::TReturnCode
 */
xAOD::TReturnCode ClusterAnalysis :: FluctuationAnalysis :: HistFinalize()
{
  std::cout << m_analysisName << " HistFinalize" << std::endl;

  return xAOD::TReturnCode::kSuccess;
}


/*
  @brief Method to analize fluctuations in Calorimeters

  Loops though the events calo distribution using a window of 
  some size and looks at caloFluctuation of all windows.
  
  Also do this for individual eta slices.

  @param1 TH2F Et distribution by (eta,phi)
  and TH1F to fill with window Et
  @param2 etaLimit (Absolute value)
  @param3 vector with caloFluctuations of eta slices.

  @return caloFluctuation of Et in that event
*/
double ClusterAnalysis :: FluctuationAnalysis :: AnalyzeFluctuations ( TH2F* h2_EtaPhi, double etaLimit, std::vector<double>& v_caloFluctuationEtaSlices){
 
  double sumWindowEt = 0;
  double sumWindowSqEt = 0;

  int nXbins =  h2_EtaPhi->GetXaxis()->GetNbins();
  int nYbins =  h2_EtaPhi->GetYaxis()->GetNbins();

  int xcorner, xBinMax;

  if( etaLimit != h2_EtaPhi->GetXaxis()->GetXmax() ){
    xcorner = h2_EtaPhi->GetXaxis()->FindBin( -etaLimit - DELTA );
    xBinMax = h2_EtaPhi->GetXaxis()->FindBin( etaLimit + DELTA) - 1;
  }
  else{
    xcorner = 1;
    xBinMax = nXbins;
  }

  int nWindows = 0;

  // nested loop moves top left corner of window around grid
  // if part of the other edge of h2 doesnt fit
  // it is not taken into account
  for(  ; xcorner <= xBinMax - m_window_Eta_size + 1; xcorner += m_window_Eta_size ){

    double sumWindowEtSlice   = 0;
    double sumWindowSqEtSlice = 0;
    double nWindowsSlice      = 0;

    for(int ycorner = 1; ycorner <= nYbins - m_window_Phi_size + 1; ycorner += m_window_Phi_size ){
      double windowEt   = 0;
      // scans that window
      for( int xbin = xcorner; xbin < xcorner + m_window_Eta_size; xbin++){
	for( int ybin = ycorner; ybin < ycorner + m_window_Phi_size; ybin++){
	  windowEt += h2_EtaPhi->GetBinContent( xbin, ybin );
	}
      }
      // total
      sumWindowEt += windowEt;
      sumWindowSqEt += (windowEt * windowEt);
      nWindows++;
      
      // eta (xcorner) slices
      sumWindowEtSlice += windowEt;
      sumWindowSqEtSlice += (windowEt * windowEt);
      nWindowsSlice++;
    }  // end ycorner loop
    
    double caloFluctuationSlice = TMath::Sqrt( sumWindowSqEtSlice / nWindowsSlice
			      - TMath::Power( sumWindowEtSlice / nWindowsSlice , 2) );
  
    v_caloFluctuationEtaSlices.push_back( caloFluctuationSlice );
  } // end xcorner loop

  double caloFluctuation = TMath::Sqrt( sumWindowSqEt / nWindows
			      - TMath::Power( sumWindowEt / nWindows , 2) );
  
  return caloFluctuation;
}
