/* 
 * Author: Michaela Queitsch-Maitland <michaela.queitsch-maitland@cern.ch>
 */

#ifndef HelperFunctions_h
#define HelperFunctions_h

#include "YKAnalysis/Global.h"

#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TEnv.h"
#include "TError.h"
#include "TRandom3.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "xAODJet/JetContainer.h"

static const float GeV = 1000.;

bool descendingPt(xAOD::Jet* a, xAOD::Jet* b);

float deltaPhi(const xAOD::Jet& j1, const xAOD::Jet& j2);
float deltaR(const xAOD::Jet &j1,const xAOD::Jet &j2);

TH1F* createHist1D(TString hname, TString title, int nbins, int xlow, int xhigh);
TH1F* createHist1D(TString hname, TString title, std::vector<double> bins);
TH2F* createHist2D(TString hname, TString title, int xnbins, float xlow, float xhigh, int ynbins, float ylow, float yhigh);
TH2F* createHist2D(TString hname, TString title, std::vector<double> xbins, std::vector<double> ybins);
TH3F* createHist3D(TString hname, TString title, int xnbins, float xlow, float xhigh, int ynbins, float ylow, float yhigh, int znbins, float zlow, float zhigh);
TH3F* createHist3D(TString hname, TString title, std::vector<double> xbins, std::vector<double> ybins, std::vector<double> zbins);

std::vector<std::string> vectorise(TString str, TString sep=" ");
std::vector<double> vectoriseD(TString str, TString sep=" ");
std::vector<double> makeUniformVec(int N, double min, double max);

#endif
