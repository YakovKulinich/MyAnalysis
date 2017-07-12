/** @file JetAnalysis.h
 *  @brief Function prototypes for JetAnalysis.
 *
 *  This contains the prototypes and members 
 *  for JetAnalysis
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef JETANALYSIS_JETANALYSIS_H
#define JETANALYSIS_JETANALYSIS_H

#include "YKAnalysis/Global.h"
#include "YKAnalysis/Analysis.h"

#include <TLorentzVector.h>

class JetContainer;

class JetCleaningTool;
class JetCalibrationTool;
class JetUncertaintiesTool;
class HIJESUncertaintyProvider;

namespace JetAnalysis{
  
  class JetAnalysis : public YKAnalysis::Analysis{
  public:
    JetAnalysis();
    JetAnalysis( const std::string& );
    virtual ~JetAnalysis();

  private:
    // these are the functions inherited from Algorithm
    virtual xAOD::TReturnCode Setup          ();
    virtual xAOD::TReturnCode HistInitialize ();
    virtual xAOD::TReturnCode Initialize     ();
    virtual xAOD::TReturnCode ProcessEvent   ();
    virtual xAOD::TReturnCode Finalize       ();
    virtual xAOD::TReturnCode HistFinalize   ();

    void UncertaintyProviderJES( const xAOD::Jet*,
				 std::vector<float>& );

    Float_t DeltaR( const xAOD::Jet* ,   
		    const xAOD::Jet* );
    
    void SaveJets( const xAOD::JetContainer* ,     // jets
		   std::vector<TLorentzVector>&,   // calib output
		   float = 0 );                    // cut 
		    
    void SaveJets( const xAOD::JetContainer*    ,  // jets
		   std::vector<TLorentzVector>& ,  // jets output vector
		   const std::string&           ,  // scale 
		   float = 0 );                    // cut 

  private:
    // For tree
    std::vector< TLorentzVector > vR_C_jets;    
    std::vector< TLorentzVector > vT_jets;      
    std::vector< TLorentzVector > vTrig_jets;   

    std::vector< std::vector< float > > v_sysUncert;
    int m_nSysUncert;
    int m_nSysUncert_pp;
    int m_nSysUncert_HI;

    std::vector< bool > v_isCleanJet; 

    // configs
    bool        m_isData        ;
    bool        m_doSystematics ;
    std::string m_recoJetAlgorithm  ; 
    std::string m_recoJetContainer  ; 
    std::string m_truthJetContainer ;
    std::string m_trigJetContainer  ;

    std::string m_calibConfig       ;       
    std::string m_calibSequence     ;    
 
    // tools
    JetCleaningTool*          m_jetCleaningTool;    
    JetCalibrationTool*       m_jetCalibrationTool; 
    JetUncertaintiesTool*     m_jetUncertaintyTool;
    HIJESUncertaintyProvider* m_hiJetUncertaintyTool;
    
    // cuts
    float m_jetPtMin ;
    float m_jetRparameter ;

  };
}

#endif
