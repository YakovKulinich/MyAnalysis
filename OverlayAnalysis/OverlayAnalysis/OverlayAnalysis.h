/** @file BaseAnalysis.h
 *  @brief Function prototypes for BaseAnalysis.
 *
 *  This contains the prototypes and members 
 *  for BaseAnalysis
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef OVERLAYANALYSIS_OVERLAYANALYSIS_H
#define OVERLAYANALYSIS_OVERLAYANALYSIS_H

#include "YKAnalysis/Global.h"
#include "YKAnalysis/Analysis.h"

#include <TrigConfxAOD/xAODConfigTool.h>
#include <TrigDecisionTool/TrigDecisionTool.h>

namespace OverlayAnalysis{
  
  class OverlayAnalysis : public YKAnalysis::Analysis{
  public:
    OverlayAnalysis();
    OverlayAnalysis( const std::string& );
    virtual ~OverlayAnalysis();

  private:
    // these are the functions inherited from Algorithm
    virtual xAOD::TReturnCode Setup          ();
    virtual xAOD::TReturnCode HistInitialize ();
    virtual xAOD::TReturnCode Initialize     ();
    virtual xAOD::TReturnCode ProcessEvent   ();
    virtual xAOD::TReturnCode Finalize       ();
    virtual xAOD::TReturnCode HistFinalize   ();


  private:
    std::string m_grlFileName;

    int  m_eventNumber;         
    int  m_LBN;                  
    int  m_runNumber;
    bool m_haveDaqError;

    std::vector< TVector3 > m_vertices;

    //-----------------------
    // Triggers
    //-----------------------
    std::string m_triggerMenu;
    std::vector < std::string >    m_v_triggers;
    std::map< std::string, bool >  m_m_passed_triggers;
    std::map< std::string, float > m_m_prescale_triggers;

    // trigger tools member variables
    Trig::TrigDecisionTool    *m_trigDecisionTool; 
    TrigConf::xAODConfigTool  *m_trigConfigTool; 

  };

}

#endif
