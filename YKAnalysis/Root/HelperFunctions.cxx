/* 
 * Author: Michaela Queitsch-Maitland <michaela.queitsch-maitland@cern.ch>
 */

#include "YKAnalysis/HelperFunctions.h"

bool descendingPt(xAOD::Jet* a, xAOD::Jet* b) { return a->pt() > b->pt(); }

float deltaPhi(const xAOD::Jet& j1, const xAOD::Jet& j2) { 
  return TVector2::Phi_mpi_pi(j1.phi()-j2.phi());
}

float deltaR(const xAOD::Jet &j1, const xAOD::Jet &j2){
  float deta = j1.eta()-j2.eta();
  float dphi = TVector2::Phi_mpi_pi(j1.phi()-j2.phi());
  return TMath::Sqrt( deta*deta+dphi*dphi );
}

TH1F* createHist1D(TString hname, TString title, int nbins, int xlow, int xhigh) {
  TH1F* h = new TH1F(hname,title,nbins,xlow,xhigh);
  return h;
}

TH1F* createHist1D(TString hname, TString title, std::vector<double> bins) {
  TH1F* h = new TH1F(hname,title,bins.size()-1,&bins[0]);
  return h;
}

TH2F* createHist2D(TString hname, TString title, int xnbins, float xlow, float xhigh, int ynbins, float ylow, float yhigh) {
  TH2F* h = new TH2F(hname,title,xnbins,xlow,xhigh,ynbins,ylow,yhigh);
  return h;
}

TH2F* createHist2D(TString hname, TString title, std::vector<double> xbins, std::vector<double> ybins) {
  TH2F* h = new TH2F(hname,title,xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0]);
  return h;
}

TH3F* createHist3D(TString hname, TString title, int xnbins, float xlow, float xhigh, int ynbins, float ylow, float yhigh, int znbins, float zlow, float zhigh) {
  TH3F* h = new TH3F(hname,title,xnbins,xlow,xhigh,ynbins,ylow,yhigh,znbins,zlow,zhigh);
  return h;
}

TH3F* createHist3D(TString hname, TString title, std::vector<double> xbins, std::vector<double> ybins, std::vector<double> zbins) {
  TH3F* h = new TH3F(hname,title,xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0],zbins.size()-1,&zbins[0]);
  return h;
}

std::vector<std::string> vectorise(TString str, TString sep) {
  std::vector<std::string> result; TObjArray *strings = str.Tokenize(sep.Data());
  if (strings->GetEntries()==0) { delete strings; return result; }
  TIter istr(strings);
  while (TObjString* os=(TObjString*)istr()) result.push_back(std::string(os->GetString()));
  delete strings; return result;
}   

std::vector<double> vectoriseD(TString str, TString sep) {
  std::vector<double> result; std::vector<std::string> vecS = vectorise(str,sep);
  for (uint i=0;i<vecS.size();++i)
    result.push_back(atof(vecS[i].c_str()));
  return result;
}   

std::vector<double> makeUniformVec(int N, double min, double max) {
  std::vector<double> vec; double dx=(max-min)/N;
  for (int i=0;i<=N;++i) vec.push_back(min+i*dx);
  return vec;
}
