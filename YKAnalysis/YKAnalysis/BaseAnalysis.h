/** @file BaseAnalysis.h
 *  @brief Function prototypes for BaseAnalysis.
 *
 *  This contains the prototypes and members 
 *  for BaseAnalysis
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef YKANALYSIS_BASEANALYSIS_H
#define YKANALYSIS_BASEANALYSIS_H

#include "YKAnalysis/Global.h"
#include "YKAnalysis/Analysis.h"

#include <GoodRunsLists/GoodRunsListSelectionTool.h>

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace YKAnalysis{
  
  class BaseAnalysis : public Analysis{
  public:
    BaseAnalysis();
    BaseAnalysis( const std::string& );
    virtual ~BaseAnalysis();

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

    //-----------------------
    // Triggers
    //-----------------------
    std::string m_triggerMenu;
    std::vector < std::string >    m_v_triggers;
    std::map< std::string, bool >  m_m_passed_triggers;
    std::map< std::string, float > m_m_prescale_triggers;

    //-----------------------
    // Tools
    //-----------------------
    GoodRunsListSelectionTool *m_grl;             

    // trigger tools member variables
    Trig::TrigDecisionTool    *m_trigDecisionTool; 
    TrigConf::xAODConfigTool  *m_trigConfigTool; 

  };

}

#endif
