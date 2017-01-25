/** @file AnalysisManager.h
 *  @brief Function prototypes for AnalysisManager.
 *
 *  This contains the prototypes and members 
 *  for AnalysisManager
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef YKANALYSIS_ANALYSISMANAGER_H
#define YKANALYSIS_ANALYSISMANAGER_H

#include "YKAnalysis/Global.h"

namespace YKAnalysis{
  
  class AnalysisManager{
  public:
    AnalysisManager();
    AnalysisManager( const std::string&, const std::string& );
    ~AnalysisManager();

    // We do not want any copies of this class
    AnalysisManager           ( const AnalysisManager& ) = delete ;
    AnalysisManager& operator=( const AnalysisManager& ) = delete ;

    int AddAnalysis ( AnalysisPtr ) ;
    int Run() ;

  private:
    xAOD::TReturnCode  Setup          ();
    xAOD::TReturnCode  EventLoop      ();

  public:

    void SetMaxEvents( int n ) { m_maxEvents = n; }

  private:
    int         m_maxEvents;
    
    std::string m_analysisName;
    std::string m_inputTreeName;

    std::string m_outputFileName;
    std::string m_configFileName;

    TChain*     m_eventChain;

    SharedData* m_sd;
    std::vector< AnalysisPtr > m_v_analysis;
  };

}

#endif
