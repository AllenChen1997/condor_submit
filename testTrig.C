#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "TH1D.h"
#include "TFile.h"
//#include "untuplizer.h" // for some unknown reason, can not load input files
//#include <TDirectory.h>
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TString.h"
#include <map>

bool isTest = false;

using namespace std;
void savehist(map<string, map<int, TH1F* > > m, bool isprint){
	for (auto iter = m.begin(); iter != m.end(); iter++){
		if (isprint) cout << iter->first << " :" << endl;
		for (auto x : iter->second){
			if (isprint) cout << "  " <<  x.first << "  ";
			x.second->Write();
		}
		if (isprint) cout << endl;
	}
}
void testTrig(string inputFile, string outfile){
	// data collection STLs
	map<string, TH1F*> PFHT_names;
	map<string, TH1F*> PFHT_names_noResCut;
	map<string, map< int, TH1F* > > PFHT_prescale_MET;
	map<string, map< int, TH1F* > > PFHT_prescale_HTwMET;
	map<string, map< int, TH1F* > > PFHT_prescale_HTnoMET;

	float HT_MET = 0;
	float HT_noMET = 0;
	vector<int> passTrigList;
	vector<int> prescaleList;
	int nPassTrig;
	// out Tree branches
	TFile* fout = new TFile(outfile.data(), "RECREATE" );
	TTree outTree("tree","out branches");
	outTree.Branch("passTrigList", &passTrigList);  
	outTree.Branch("prescaleList", &prescaleList);  
	outTree.Branch("HT_MET", &HT_MET);  
	outTree.Branch("HT_noMET", &HT_noMET);  
	outTree.Branch("nPassTrig", &nPassTrig);  
	
	string line;
	ifstream flist(inputFile.data() );
	while (getline(flist, line) ){
		cout << "reading " << line << endl;
		//get TTree from file ...
		TFile* myFile;
		myFile = TFile::Open(line.data(), "READONLY" );
		//TDirectory* td = (TDirectory*) myFile->Get("tree");
		TTreeReader data("tree/treeMaker",myFile);
		TTreeReaderValue< Int_t > nVtx(data, "nVtx");
		TTreeReaderArray< string > trigName(data, "hlt_trigName");
		TTreeReaderArray< bool > trigResult(data, "hlt_trigResult");
		TTreeReaderArray< int > prescale(data, "hlt_trigPrescale");
		TTreeReaderValue< Float_t > METPT(data, "pfMetCorrPt");
		  // thinJet //
		TTreeReaderValue< int > THINnJet(data, "THINnJet");
		TTreeReaderArray< float > THINjetPx(data, "THINjetPx");
		TTreeReaderArray< float > THINjetPy(data, "THINjetPy");
		TTreeReaderArray< float > THINjetPz(data, "THINjetPz");
		TTreeReaderArray< float > THINjetEnergy(data, "THINjetEnergy");
		
		Long64_t nPass[20]={0};
		int total_entry = data.GetEntries(true);
		int jEntry = 0;

		while (data.Next() ){
			jEntry++;
			cout << "\r";
			if (isTest) cout <<  "Processing event " << jEntry << " of " << total_entry << endl;
			else {
				if (jEntry % 1000 == 0) cout <<  "Processing event " << jEntry << " of " << total_entry << flush;
				else if (jEntry == total_entry) cout <<  "Processing event " << jEntry << " of " << total_entry << flush;
			}
			if (isTest) if(jEntry>100)break;
			//0. has a good vertex
			if(*nVtx<1)continue;
			nPass[0]++;

			//1. ak4Jet
			vector<TLorentzVector> ak4_eta_3p0_pt_20; 
			for (int ijet = 0; ijet < *THINnJet; ijet++){
				TLorentzVector tmpTL;
				tmpTL.SetPxPyPzE(THINjetPx[ijet],THINjetPy[ijet],THINjetPz[ijet],THINjetEnergy[ijet]);
				float absjetEta = TMath::Abs(tmpTL.Eta() );
				float jetPt = tmpTL.Pt();
				if (absjetEta >= 3.0) continue;
				if (jetPt <= 20) continue;
				// collect ak4 jet pass pt, eta cut. for getting offline HT = MET + ak4J or = ak4J only
				ak4_eta_3p0_pt_20.push_back(tmpTL);
			}
			// actually this part can combine into the for loop above, but I keep vector ak4_eta_3p0_pt_20 for probabily others usage.
			HT_noMET = 0;
			for (auto x : ak4_eta_3p0_pt_20){ 
				HT_noMET += x.Pt();
			}
			HT_MET = HT_noMET + *METPT;
			
			//2. trigger 
			bool passTrigger=false;
			//vector<string> aPassTrigList;
			int it = -1;
			for(auto iter=trigResult.begin(); iter< trigResult.end(); iter++)
			{
				it++;
				string thisTrig= trigName[it];
				bool results = trigResult[it];
				// cuts for trigger: trig name, trig result //
				if(thisTrig.find("HLT_PFHT")== std::string::npos)continue;
				if(thisTrig.size() > 20 ) continue;
				// pick the trigger path name: HLT_PFHT180_v8 ---> PFHT180 //
				stringstream ss(thisTrig);
				string trigPath;
				int counterT = 0;
				while(getline(ss,trigPath,'_')){
					counterT++;
					if (counterT == 2) break; // the key word always in the second place by the delimiter "_"
				} // now trigPath should be PFHT180 or some other numbers 370, 390 ...
				ss.clear();
				// check all prescale distribution in each triger path // 
				int iprescale = prescale[it];
				if (PFHT_names[trigPath] == nullptr){
					TH1F* tmph = new TH1F(trigPath.data(),trigPath.data() ,110,0,1100);
					PFHT_names[trigPath] = tmph;
					string newName = trigPath+"_";
					TH1F* tmph2 = (TH1F*)tmph->Clone(newName.data());
					PFHT_names_noResCut[trigPath] = tmph2;
				}
				PFHT_names_noResCut[trigPath]->Fill(iprescale);
				
				if (results != 1) continue;
				PFHT_names[trigPath]->Fill(iprescale);
				// to look MET/HT distribution in each path and prescale //
				if (PFHT_prescale_MET[trigPath][iprescale] == nullptr){
					TString ts = trigPath + "_pre" + iprescale;
					TH1F* tmph = new TH1F(ts+"_MET","METPT",120,0,1200);
					PFHT_prescale_MET[trigPath][iprescale] = tmph;
					TH1F* tmph2 = new TH1F(ts+"_HTwMET","HT(with MET)",120,0,1200);
					PFHT_prescale_HTwMET[trigPath][iprescale] = tmph2;
					TH1F* tmph3 = new TH1F(ts+"_HTnoMET","HT(no MET)",120,0,1200);
					PFHT_prescale_HTnoMET[trigPath][iprescale] = tmph3;
				}
				PFHT_prescale_MET[trigPath][iprescale ]->Fill(*METPT);
				PFHT_prescale_HTwMET[trigPath][iprescale ]->Fill(HT_MET);
				PFHT_prescale_HTnoMET[trigPath][iprescale ]->Fill(HT_noMET);
				// for more convinent to use in the next step code. use TString to remove prefix PFHT, and than convert to int. 
				TString ts = trigPath; 
				ts.Remove(0,4);
				if (isTest) cout << ts.Data() << endl;
				stringstream sb;
				sb << ts.Data();
				int trigNum;
				sb >> trigNum;
				if (isTest) cout << "Num " << trigNum << endl;
				sb.clear();
				passTrigList.push_back(trigNum);
				prescaleList.push_back(iprescale);
			} // end of triggers	
			
			nPassTrig = passTrigList.size();
			if (nPassTrig == 0 ) continue; // we will not store event without any pass trigger 
			outTree.Fill();
			passTrigList.clear();
			prescaleList.clear();

		}// end of entries
		cout << endl; // change one line, for the output screan style
	}// end of loop files
	// output files
	fout->cd();
	
	// print out the list of prescale
	cout << endl;
	cout << " ////// pre-scale list ///// " << endl;
	savehist(PFHT_prescale_MET,true);
	savehist(PFHT_prescale_HTwMET,false);
	savehist(PFHT_prescale_HTnoMET,false);

	for (auto x : PFHT_names)  x.second->Write();
	for (auto x : PFHT_names_noResCut)  x.second->Write();
	outTree.Write();
	fout->Close();
	  
}
