/** @file FluctuationAnalysis.h
 *  @brief Function prototypes for FluctuationAnalysis.
 *
 *  This contains the prototypes and members 
 *  for FluctuationAnalysis
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef CLUSTERANALYSIS_FLUCTUATIONANALYSIS_H
#define CLUSTERANALYSIS_FLUCTUATIONANALYSIS_H

#include "YKAnalysis/Global.h"
#include "YKAnalysis/Analysis.h"

namespace ClusterAnalysis{
  
  class FluctuationAnalysis : public YKAnalysis::Analysis{
  public:
    FluctuationAnalysis();
    FluctuationAnalysis( const std::string& );
    virtual ~FluctuationAnalysis();

  private:
    // these are the functions inherited from Algorithm
    virtual xAOD::TReturnCode Setup          ();
    virtual xAOD::TReturnCode HistInitialize ();
    virtual xAOD::TReturnCode Initialize     ();
    virtual xAOD::TReturnCode ProcessEvent   ();
    virtual xAOD::TReturnCode Finalize       ();
    virtual xAOD::TReturnCode HistFinalize   ();

    double AnalyzeFluctuations ( TH2F*, double, std::vector<double>&);

  private:
    // For tree
    double m_FCalEt;
    double m_caloFluctuation;
    std::vector< double > m_v_caloFluctuationEtaSlices;

    // Histograms
    TH3D* h3_EtaPhiFCalEt;
    TH3D* h3_EtaPhiFCalEt_UW;
    
    TH1D* h1_FCalEt;

    std::string m_clusterContainerName;

    // fluctuation window
    int m_window_Eta_size;
    int m_window_Phi_size;

    // for binning
    double m_etaMin, m_etaMax, m_phiMin, m_phiMax;
    double m_fCalEtMin, m_fCalEtMax;
    int    m_nEtaBins, m_nPhiBins, m_nFCalEtBins;

    const double DELTA = 0.001;

  public:
    int GetWindow_Eta_size() { return m_window_Eta_size; }
    int GetWindow_Phi_size() { return m_window_Phi_size; }

    void SetWindow_Eta_size( int x ) { m_window_Eta_size = x; }
    void SetWindow_Phi_size( int y ) { m_window_Phi_size = y; }
  };

}

#endif
