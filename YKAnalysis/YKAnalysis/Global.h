/** @file Global.h
 *  @brief Global declarations.
 *
 *  This contains the forward declarations
 *  and typedefs for other classes to use.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef YKANALYSIS_GLOBAL_H
#define YKANALYSIS_GLOBAL_H

#include <memory>
#include <vector>
#include <string>

#include "YKAnalysis/SharedData.h"
#include "YKAnalysis/HelperFunctions.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "AsgTools/ToolHandle.h"
#include "AsgTools/MessageCheck.h"

class TEnv;
class TChain;
class TFile;
class TTree;
class TH1;
class TH1F;
class TH2F;
class TH3F;

namespace xAOD{
  class TEvent;
  class TReturnCode;
}

namespace YKAnalysis{
  // forward declarations
  class Analysis;
  class AnalysisManager;
  class BaseAnalysis;
  // types
  typedef std::shared_ptr<Analysis> AnalysisPtr; 
}

// Helper macro for checking xAOD::TReturnCode return values
#define CHECK_STATUS( CONTEXT, EXP )			\
  do {							\
    if( ! EXP.isSuccess() ) {				\
      Error( CONTEXT,					\
	     XAOD_MESSAGE( "Failed to execute: %s" ),	\
	     #EXP );					\
      abort();						\
    }							\
  } while( false )

#endif
