/** @file Analysis.h
 *  @brief Function prototypes for Analysis (abstract).
 *
 *  This contains the prototypes for and members
 *  for abstract class Analysis.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef YKANALYSIS_ANALYSIS_H
#define YKANALYSIS_ANALYSIS_H

#include "YKAnalysis/Global.h"

#include <TEnv.h>

#include <iostream>

namespace YKAnalysis{

  class Analysis
  { 
  public:
    // this is a destructor
    Analysis() : m_analysisName( "" ), m_sd(NULL) {}
    Analysis( const std::string& name ) : m_analysisName( name ), m_sd(NULL) {}
    virtual ~Analysis() {};
    
  public:
    // these are the functions inherited from Algorithm
    virtual xAOD::TReturnCode Setup          () = 0;
    virtual xAOD::TReturnCode HistInitialize () = 0;
    virtual xAOD::TReturnCode Initialize     () = 0;
    virtual xAOD::TReturnCode ProcessEvent   () = 0;
    virtual xAOD::TReturnCode Finalize       () = 0;
    virtual xAOD::TReturnCode HistFinalize   () = 0;

    void  RegisterSharedData ( SharedData* sd ) { m_sd = sd; }
   
    const std::string& GetAnalysisName() const { return m_analysisName; }

  protected:
    std::string m_analysisName ;

    SharedData* m_sd;
  };

}

#endif
