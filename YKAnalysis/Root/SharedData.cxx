/** @file SharedData.cxx
 *  @brief Implementation of SharedData.
 *
 *  SharedData class holds data common to analysis packages.
 *  This includes histograms, trees, event counter.
 *  Analysis packages will have a pointer to this class passed from
 *  the manager.
 * 
 *  @author Yakov Kulinich
 *  @bug No known bugs.
 */
#include "YKAnalysis/SharedData.h"

/** @brief Default Constructor for SharedData.
 */
YKAnalysis :: SharedData :: SharedData ()
  :  m_eventStore(NULL), 
     m_eventCounter(0),      
     m_outputFileName( "" ), 
     m_configFileName( "" ),
     m_fout(NULL),
     m_tree(NULL),
     m_config(NULL),
     m_hEventStatistics(NULL)
{}

/** @brief Constructor for SharedData.
 *
 *  Constructs an SharedData object.
 *
 *  @param1 string with output filename
 *  @param2 string with name of config file
 */
YKAnalysis :: SharedData :: SharedData ( const std::string& outputFileName,
					 const std::string& configFileName )
  :  m_eventStore(NULL), 
     m_eventCounter(0),      
     m_outputFileName( outputFileName ), 
     m_configFileName( configFileName ),
     m_fout(NULL),
     m_tree(NULL),
     m_config(NULL),
     m_hEventStatistics(NULL)
{}

/** @brief Destructor for SharedData.
 *
 *  Cleans up an SharedData object.
 */
YKAnalysis :: SharedData :: ~SharedData() 
{
  delete m_eventStore;
  delete m_tree;
  delete m_fout;
  delete m_config;
}

/** @brief Function to add an event store.
 *
 *  Initialize TFile, Tree, TEnv (config) 
 *
 *  @return void
 */
static const int n_eventStatistics = 10;

void YKAnalysis :: SharedData :: Initialize()
{
  m_fout         = new TFile( m_outputFileName.c_str(), "RECREATE" );
  m_tree         = new TTree( "tree"                  , "tree"     );

  m_config       = new TEnv ();
  m_config->ReadFile( m_configFileName.c_str(), EEnvLevel(0));

  std::cout << m_config << " " << m_configFileName << std::endl;

  m_hEventStatistics = new TH1D( "hEventStatistics","hEventStatistics", 
				 n_eventStatistics, 0, n_eventStatistics );
}

/** @brief Function to add an event store.
 *
 *  Checks to see if there is already an event store. 
 *  If there is not, it is added.
 *
 *  @param1 Pointer to TEvent
 *
 *  @return void
 */
void YKAnalysis :: SharedData :: AddEventStore( xAOD::TEvent* ev ) 
{
  if( !m_eventStore ) m_eventStore = ev; 
}

/** @brief Function to add an output histo.
 *
 *  @param1 Pointer to histogram
 *
 *  @return void
 */
void YKAnalysis :: SharedData :: AddOutputHistogram( TH1* h )
{
  m_v_hists.push_back( h );
}
/** @brief End of event
 *
 *  Fill the tree if we had a good event. 
 *  Increment event counter 
 *
 *  @return void 
 */
void YKAnalysis :: SharedData :: EndOfEvent( bool goodEvent )
{
  if( goodEvent ) m_tree->Fill();
  m_eventCounter++;
}

/** @brief Function to check if printing necessary
 *
 *  Print every event until 10, then every 10,
 *  then every 100, then every 1000, etc.
 *
 *  @return bool to print or not
 */
bool YKAnalysis :: SharedData :: DoPrint() 
{
  int statSize=1;
  if( m_eventCounter != 0){
    double power=std::floor(log10(m_eventCounter));
    statSize=(int)std::pow(10.,power);
  }
  if(m_eventCounter%statSize==0) return true;
  return false;
}

/** @brief Finalize Shared Data
    
    Writes histos and closes the TFile

    can add statistics histo to m_v_histos
    but since its different (and used amongst all analysis)
    just leaving it like this for now.

    @return void
 */
void YKAnalysis :: SharedData :: Finalize() 
{
  m_fout->cd();

  // write tree
  m_tree->Write();

  // write all histos from various analysis
  for( auto& h : m_v_hists ) { h->Write(); }
  
  // write common statistics histo
  m_hEventStatistics->Write();

  m_fout->Close();
}
