#include "TH1F.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "iostream"
#include "TString.h"

using namespace std;
void load_hist(string inputfilename, TH1F*& h_SDmass, TH1F*& h_nFat, TH1F*& h_ddb){
        TFile* fin;
        fin = TFile::Open(inputfilename.data() );
        TDirectory* td = (TDirectory*) fin->Get("tree");
		TTreeReader myRead("treeMaker",td);
		TTreeReaderValue<Int_t> nFat(myRead,"FATnJet");
		TTreeReaderArray<Float_t> FATjetSDmass(myRead,"FATjetSDmass");
		TTreeReaderArray<Float_t> FATjet_probHbb(myRead, "FATjet_probHbb");

		int total_entries = myRead.GetEntries(true);
		while(myRead.Next()){ // start loop entries
			for (auto x:FATjetSDmass){
				h_SDmass->Fill(x);
			}
			for (auto x:FATjet_probHbb){
				h_ddb->Fill(x);
			}
			h_nFat->Fill(*nFat);
		} // end of entries loop
			
}

void readElement(string filelist = "inputlist.txt", string outputname = "test.root"){
	// setup the necessary histograms //
	TH1F* h_SDmass = new TH1F("SDmass","",50,0,200);
	TH1F* h_nFat = new TH1F("nFat","",10,0,10);
	TH1F* h_ddb = new TH1F("ddb","FJetCSV",10,0,1);
	// check the input type //
	TString inputname = filelist;
	if (inputname.Contains(".root") ) {
		load_hist(filelist, h_SDmass,h_nFat,h_ddb);
	} else {
		string line;
		ifstream flist(filelist.data());
		int i=0;
		while(getline(flist,line) ){
			i++;
			cout << "reading file # " << i << endl;
			load_hist(line,h_SDmass,h_nFat,h_ddb);
		}
	}
	// output histo //
	TFile* fout = new TFile(outputname.data(),"recreate");
	h_SDmass->Write();
	h_nFat->Write();
	h_ddb->Write();
	fout->Close();
}
		
	
