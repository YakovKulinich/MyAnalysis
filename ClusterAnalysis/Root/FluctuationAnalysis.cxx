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
  m_etaMin   = -5;             m_etaMax = 5;      
  
  m_nPhiBins = 64;
  m_phiMin   = -TMath::Pi();   m_phiMax = TMath::Pi(); 

  m_nFCalEtBins = 600 ; 
  m_fCalEtMin   = 0;           m_fCalEtMax = 6 ;  // TeV
  
  m_nWindowEtBins = 250;
  m_windowEtMin   = 0;         m_windowEtMax   = 250;  // GeV
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

  m_nFCalEtBins = 600 ; 
  m_fCalEtMin = 0;           m_fCalEtMax = 6 ;  // TeV

  m_nWindowEtBins = 250;
  m_windowEtMin   = 0;         m_windowEtMax   = 250;  // GeV
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

  m_v_etaLimits.push_back( 0.7 );
  m_v_etaLimits.push_back( 1.4 );
  m_v_etaLimits.push_back( 2.1 );
  m_v_etaLimits.push_back( 2.8 );

  for( auto& etaLimit : m_v_etaLimits ) 
    std::cout << "EtaLimit = " << etaLimit << std::endl;

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

  h3_EtaFCalEtWindowEt = new TH3D("h3_EtaFCalEtWindowEt",";#eta;#SigmaE_{T} (3.2<|#eta|<4.6) [TeV];#SigmaE_{T} Window",
				  m_nEtaBins, m_etaMin, m_etaMax,
				  m_nFCalEtBins, m_fCalEtMin, m_fCalEtMax,
				  m_nWindowEtBins, m_windowEtMin, m_windowEtMax ); // for 7x7 window for now
  h3_EtaFCalEtWindowEt->Sumw2();
  m_sd->AddOutputHistogram( h3_EtaFCalEtWindowEt );

  m_sd->AddOutputToTree< double >( "FCalEt", &m_FCalEt );
  m_sd->AddOutputToTree< std::vector< double > >( "v_caloFluctuations", &m_v_caloFluctuations );

  m_sd->AddOutputToTree< std::vector< double > >( "v_caloFluctuationEtaSlices", &m_v_caloFluctuationEtaSlices );

  // FCalEt
  h1_FCalEt  = new TH1D("h1_FCalEt", ";#SigmaE_{T} (3.2<|#eta|<4.6) [TeV];Entries", 
			m_nFCalEtBins * 10, m_fCalEtMin, m_fCalEtMax);
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
  TH2D h2_EtaPhi("h2_EtaPhi","h2_EtaPhi",  m_nEtaBins, m_etaMin, m_etaMax,  m_nPhiBins, m_phiMin, m_phiMax);

  // loop over cluster container
  for(const auto* caloCluster : *caloClusterContainer){
    double cc_Eta  = caloCluster->eta();              // Phi
    double cc_Phi  = caloCluster->phi();              // Phi
    double cc_E    = caloCluster->e() * 0.001;        // E in GeV
    double cc_Et   = cc_E / TMath::CosH( cc_Eta );    // Et in GeV

    h2_EtaPhi.Fill( cc_Eta, cc_Phi, cc_Et );  // temp histo to be sent to AnalyzeFluctiations
  } // end for loop over cluster
  
  m_v_caloFluctuationEtaSlices.clear();
  AnalyzeFluctuationsEtaSlices( &h2_EtaPhi, m_v_etaLimits.back(), 
				m_v_caloFluctuationEtaSlices, h3_EtaFCalEtWindowEt );  

  m_v_caloFluctuations.clear(); 
  for( auto& etaLimit : m_v_etaLimits ){
    m_v_caloFluctuations.
      push_back( AnalyzeFluctuations( &h2_EtaPhi, etaLimit ) );
  }
  
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
  
  @param1 TH2D Et distribution by (eta,phi)
  and TH1F to fill with window Et
  @param2 etaLimit (Absolute value)

  @return caloFluctuation of Et in that event
*/
double ClusterAnalysis :: FluctuationAnalysis :: AnalyzeFluctuations ( TH2D* h2_EtaPhi, double etaLimit ){
  int nXbins =  h2_EtaPhi->GetXaxis()->GetNbins();
  int nYbins =  h2_EtaPhi->GetYaxis()->GetNbins();

  int xcorner, xBinMax;

  if( etaLimit != h2_EtaPhi->GetXaxis()->GetXmax() ){
    xcorner = h2_EtaPhi->GetXaxis()->FindBin( -etaLimit + DELTA );
    xBinMax = h2_EtaPhi->GetXaxis()->FindBin(  etaLimit + DELTA ) - 1;
  }
  else{
    xcorner = 1;
    xBinMax = nXbins;
  }

  double sumWindowEt   = 0;
  double sumWindowSqEt = 0;
  int    nWindows      = 0;
   
  // nested loop moves top left corner of window around grid
  // if part of the other edge of h2 doesnt fit
  // it is not taken into account
  for(  ; xcorner <= xBinMax - m_window_Eta_size + 1; xcorner += m_window_Eta_size ){
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
    }  // end ycorner loop
  } // end xcorner loop

  double caloFluctuation =
    TMath::Sqrt( sumWindowSqEt / nWindows
		 - TMath::Power( sumWindowEt / nWindows , 2) );
  
  if( m_sd->DoPrint() ){
    std::cerr << "   etaSize       = " << m_window_Eta_size << std::endl;
    std::cerr << "   phiSize       = " << m_window_Phi_size << std::endl;
    std::cerr << "   sumWindowEt   = " << sumWindowEt << std::endl;
    std::cerr << "   sumWindowSqEt = " << sumWindowSqEt << std::endl;
    std::cerr << "   nWindows      = " << nWindows << std::endl;
    std::cerr << "   caloFluc      = " << caloFluctuation << std::endl;
    std::cerr << "   FCalEt        = " << m_FCalEt << std::endl;
  }

  return caloFluctuation;
}

/*
  @brief Method to analize fluctuations in Calorimeters

  Loops though the events calo distribution using a window of 
  some size and looks at caloFluctuation of all windows.
  
  Also do this for individual eta slices.

  @param1 TH2D Et distribution by (eta,phi)
  and TH1F to fill with window Et
  @param2 etaLimit (Absolute value)
  @param3 vector with caloFluctuations of eta slices.

  @return caloFluctuation of Et in that event
*/
double ClusterAnalysis :: FluctuationAnalysis :: AnalyzeFluctuationsEtaSlices
( TH2D* h2_EtaPhi, double etaLimit, std::vector<double>& v_caloFluctuationEtaSlices, TH3D* h3_EtaFCalEtWindowEt ){
  int nXbins =  h2_EtaPhi->GetXaxis()->GetNbins();
  int nYbins =  h2_EtaPhi->GetYaxis()->GetNbins();

  int xcorner, xBinMax;

  if( etaLimit != h2_EtaPhi->GetXaxis()->GetXmax() ){
    xcorner = h2_EtaPhi->GetXaxis()->FindBin( -etaLimit + DELTA );
    xBinMax = h2_EtaPhi->GetXaxis()->FindBin(  etaLimit + DELTA ) - 1;
  }
  else{
    xcorner = 1;
    xBinMax = nXbins;
  }

  double sumWindowEt   = 0;
  double sumWindowSqEt = 0;
  int    nWindows      = 0;
   
  // nested loop moves top left corner of window around grid
  // if part of the other edge of h2 doesnt fit
  // it is not taken into account
  for(  ; xcorner <= xBinMax - m_window_Eta_size + 1; xcorner += m_window_Eta_size ){

    double sumWindowEtSlice   = 0;
    double sumWindowSqEtSlice = 0;
    int    nWindowsSlice      = 0;

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
      
      // fill 3d histo
      double etaValue = h3_EtaFCalEtWindowEt->GetXaxis()->GetBinCenter( xcorner );
      h3_EtaFCalEtWindowEt->Fill( etaValue, m_FCalEt, windowEt );

      // eta (xcorner) slices
      sumWindowEtSlice += windowEt;
      sumWindowSqEtSlice += (windowEt * windowEt);
      nWindowsSlice++;
    }  // end ycorner loop
    
    double caloFluctuationSlice = 
      TMath::Sqrt( sumWindowSqEtSlice / nWindowsSlice
		   - TMath::Power( sumWindowEtSlice / nWindowsSlice , 2) );
    
    v_caloFluctuationEtaSlices.push_back( caloFluctuationSlice );
  } // end xcorner loop

  double caloFluctuation =
    TMath::Sqrt( sumWindowSqEt / nWindows
		 - TMath::Power( sumWindowEt / nWindows , 2) );
  
  if( m_sd->DoPrint() ){
    std::cerr << "   etaSize       = " << m_window_Eta_size << std::endl;
    std::cerr << "   phiSize       = " << m_window_Phi_size << std::endl;
    std::cerr << "   sumWindowEt   = " << sumWindowEt << std::endl;
    std::cerr << "   sumWindowSqEt = " << sumWindowSqEt << std::endl;
    std::cerr << "   nWindows      = " << nWindows << std::endl;
    std::cerr << "   caloFluc      = " << caloFluctuation << std::endl;
    std::cerr << "   FCalEt        = " << m_FCalEt << std::endl;
  }

  return caloFluctuation;
}
