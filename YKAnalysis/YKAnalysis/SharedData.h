/** @file SharedData.h
 *  @brief Function prototypes for SharedData.
 *
 *  This contains the prototypes and members 
 *  for SharedData.
 *
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */

#ifndef YKANALYSIS_SHAREDDATA_H
#define YKANALYSIS_SHAREDDATA_H

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include <TEnv.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TLorentzVector.h>

#include <iostream>
#include <string>

namespace YKAnalysis{
  
  class SharedData{
    
  public:
    SharedData();
    SharedData( const std::string&, const std::string& );
    ~SharedData();

    // We do not want any copies of this class
    SharedData            ( const SharedData& ) = delete ;
    SharedData& operator= ( const SharedData& ) = delete ;

    void Initialize();

    void AddEventStore          ( xAOD::TEvent* ); 
    xAOD::TEvent* GetEventStore () { return m_eventStore; };  
 
    template<class T> 
    void   AddOutputToTree    ( const std::string&, T*);
    void   AddOutputHistogram ( TH1* );
   
    int    GetEventCounter    () { return m_eventCounter; }

    TEnv*  GetConfig          () { return m_config; }

    TH1*   GetEventStatistics () { return m_hEventStatistics; }

    void   EndOfEvent       ( bool );

    bool   DoPrint          ();

    void   Finalize         ();

  private:
    xAOD::TEvent* m_eventStore;
    
    int           m_eventCounter;

    std::string   m_outputFileName;
    std::string   m_configFileName;

    TFile*        m_fout;
    TTree*        m_tree;
    TEnv*         m_config;

    std::vector< TH1* > m_v_hists;

    TH1*          m_hEventStatistics;
  };

}

/** @brief Function to add an object to the tree
 *
 *  @param1 Name of branch
 *  @param2 Pointer to object
 *
 *  @return void
 */
template<class T> 
void YKAnalysis :: SharedData :: AddOutputToTree( const std::string& name, T* pObj ){
  std::cout << "Adding " << name << std::endl;
  m_tree->Branch( name.c_str(), pObj );
}

#endif
