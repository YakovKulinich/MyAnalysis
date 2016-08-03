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

    virtual Float_t DeltaR( const xAOD::Jet* ,   
			    const xAOD::Jet* );

    // save just the reco jets
    virtual void SaveJets( float                     ,  
			   const xAOD::JetContainer* , // reco
			   const xAOD::JetContainer* , // calib reco	
			   std::vector<bool>&            , // jetCleaning 
			   std::vector<TLorentzVector>&  , // default output
			   std::vector<TLorentzVector>&  , // em output
			   std::vector<TLorentzVector>&  , // calib output
			   std::vector<bool>&            );// jetCleaning output
  
    virtual void SaveJets( float                          ,
			   const xAOD::JetContainer *     ,   // reco
			   std::vector<TLorentzVector>&   );  // reco output vector  

    virtual void SaveJets( float                          ,  
			   const xAOD::JetContainer *     ,   // reco
			   std::vector<TLorentzVector>&   ,   // reco output vector  
			   const std::string&             );

  private:
    // For tree
    std::vector<TLorentzVector> vT_jets;      

    std::vector<TLorentzVector> vR_C_jets;    
    std::vector<TLorentzVector> vR_EM_jets;   
    std::vector<TLorentzVector> vR_DF_jets;   

    std::vector<bool>           v_isCleanJet; 

    // configs
    bool        m_isData;
    std::string m_recoJetAlgorithm  ; 
    std::string m_recoJetContainer  ; 
    std::string m_truthJetContainer ;
    std::string m_calibConfig       ;       
    std::string m_calibSequence     ;    
 
    // tools
    JetCleaningTool*    m_jetCleaningTool;    
    JetCalibrationTool* m_jetCalibrationTool; 
    JetCalibrationTool* m_jetCalibration_insitu;

    // cuts
    float m_jetPtMin ;
    float m_jetRparameter ;

  };
}

#endif
