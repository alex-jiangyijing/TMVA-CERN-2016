nclude"TFile.h"
#include"TTree.h"
#include"TMVA/Tools.h"
#include"TMVA/Factory.h"
#include"TH1I.h"
#include"TH1D.h"
#include<vector>
#include <iostream>
using namespace TMVA;
using namespace std;
int mvafactory(vector<int>& combination1, TTree* sig=0, TTree* bkg=0, int type=1, int kfold=0){
	TFile* outputFile = new TFile( "output/TMVA.root", "RECREATE" );

	Factory* factory= new Factory("vbfgammaSlient", outputFile, "");
	double sigWeight = 1.0;        
	double bkgWeight = 1.0;
	vector<int> combination2; 
	vector<int> combination3;
	vector<int> combination4;


	TCut testCut = "EventNumber%2==0";
	TCut trainCut = "EventNumber%2==1";
	if(kfold==1){
		testCut = "EventNumber%2==1";
		trainCut = "EventNumber%2==0";
	}

	//run mva
	factory->AddTree(sig, "Signal", sigWeight, trainCut, "train");
	factory->AddTree(sig, "Signal", sigWeight, testCut, "test");
	factory->AddTree(bkg, "Background", bkgWeight, trainCut, "train");
	factory->AddTree(bkg, "Background", bkgWeight, testCut, "test");
	factory->SetWeightExpression("EventWeight"); 

	//numbering the variables
	vector<string> variables;
	variables.push_back("HT_soft");
	variables.push_back("dRB1Ph");
	variables.push_back("dRB2Ph");
	variables.push_back("dRJ1Ph");
	variables.push_back("dRJ2Ph");
	variables.push_back("pTBal");
	variables.push_back("WidthJ1");
	variables.push_back("WidthJ2");
        variables.push_back("cenPhJJ");
	variables.push_back("dPhiPhBmin");
	variables.push_back("dRB1J1");
	variables.push_back("dRB1J2");
	variables.push_back("dRB2J1");
	variables.push_back("dRB2J2");
	variables.push_back("dPhiJJ");
	variables.push_back("cenHJJ");
	variables.push_back("cenHgJJ");
	variables.push_back("pTBB");
	variables.push_back("pTPh");
	variables.push_back("cosThetaC");
	variables.push_back("cosThetaA");
	variables.push_back("pTB2");
	variables.push_back("pTJ1");
	variables.push_back("pTJ2");
	variables.push_back("dRJJ");
	variables.push_back("etaJ1");
	variables.push_back("etaJ2");
	variables.push_back("etaB1");
	variables.push_back("etaB2");
	variables.push_back("etaPh");
        //fixed
	variables.push_back("mJJ");
        variables.push_back("dEtaJJ");
        //spectator
	variables.push_back("mBB");







	//user's choice
	int number_of_variables=30;
	int size1=combination1.size();
	//set parameters
	
	TCut mycut = "tagCatExcl==2";
	mycut += "mJJ>800000 && pTPh>30000 && pTBB>80000";

	mycut += "passTrig && matchTrig";

	int nTrain_Signal = 0;
	int nTest_Signal = 0;
	int nTrain_Background = 0;
	int nTest_Background = 0;
	TH1I no_repeat("no_repeat","",number_of_variables,0,number_of_variables);
	
	if (type==0) {
		//initialize the first generation
		for(int i=9;i<18;i++)
		{
			combination1.push_back(i);
			factory->AddVariable(variables.at(i), 'F');
		}
	}else if (type==1){
		for(int i=0;i<size1;i++)
		{
			factory->AddVariable(variables.at(combination1[i]));
			cout<<"YJ combination1:   "<<combination1[i]<<endl;
		}
	}
	else{

		for(int i=0;i<number_of_variables;i++)
		{
			no_repeat.SetBinContent(i+1,1);
			for(int j=0;j<size1;j++)
			{
				if(i==combination1[j])no_repeat.SetBinContent(i+1,0);
			}

		}
		if (type==2){
			int type1_location=(int)gRandom->Uniform(0,size1);
			
			int type1_substitute=no_repeat.GetRandom();
			for(int i=0;i<type1_location;i++)
			{
				combination2.push_back(combination1[i]);
			cout<<"YJ combination2:   "<<combination2[i]<<endl;
				factory->AddVariable(variables.at(combination2[i]), 'F');
			}

			combination2.push_back(type1_substitute);
			factory->AddVariable(variables.at(type1_substitute), 'F');
			cout<<"YJ combination2:   "<<combination2[type1_location]<<endl;

			for(int i=type1_location+1;i<size1;i++)
			{
				combination2.push_back(combination1[i]);
			cout<<"YJ combination2:   "<<combination2[i]<<endl;
				factory->AddVariable(variables.at(combination2[i]), 'F');
			}
			combination1 = combination2;

		}else if (type==3){
			int plus=no_repeat.GetRandom();
			cout<<"plus: "<<plus<<endl;
			for(int i=0;i<size1;i++)
			{
				combination3.push_back(combination1[i]);
				cout<<"YJ combination3:   "<<combination3[i]<<endl;
				factory->AddVariable(variables.at(combination1[i]),'F');
			}
			combination3.push_back(plus);
			cout<<"YJ combination3:   "<<combination3[size1]<<endl;
			factory->AddVariable(variables.at(combination3[size1]),'F');
			combination1 = combination3;

		} else if (type==4){
			int minus=(int)gRandom->Uniform(0,size1);
			cout<<"minus"<<minus<<endl;
			for(int i=0;i<minus;i++)
			{
				combination4.push_back(combination1[i]);
				cout<<"YJ combination4:   "<<combination4[i]<<endl;
				factory->AddVariable(variables.at(combination1[i]), 'F');
			}
			for(int i=minus+1;i<size1;i++)
			{
				combination4.push_back(combination1[i]);
				cout<<"YJ combination4:   "<<combination4[i-1]<<endl;
				factory->AddVariable(variables.at(combination1[i]), 'F');
			}
			combination1 = combination4;
		}
	}
        //fixed
        factory->AddVariable(variables.at(30),'F');
        factory->AddVariable(variables.at(31),'F');
        //for the selection
	factory->AddSpectator(variables.at(32),'F');
	TString splitMode = "Random"; 
	factory->PrepareTrainingAndTestTree(mycut, nTrain_Signal, nTrain_Background, nTest_Signal, nTest_Background, splitMode);
	factory->BookMethod(TMVA::Types::kBDT, "BDT", "NTrees=200:BoostType=AdaBoost");   
	factory->TrainAllMethods();
	factory->TestAllMethods();
	factory->EvaluateAllMethods();
	outputFile->Close();
	delete factory, outputFile;
	return combination1.size();

};

double mvasn(){
	TFile* fi =TFile::Open( "output/TMVA.root" );
	TTree* TestTree=(TTree*)fi->Get("TestTree");
	TH1D* count = new TH1D("count","",2,0,2);

	TestTree->Draw("classID>>count","(mBB>100000&&mBB<150000&&BDT>0.2)*weight","goff");
	double Signal1=count->GetBinContent(1);
	double Background1=count->GetBinContent(2);
	double S1=Signal1/sqrt(Signal1+Background1);

	delete fi, TestTree, count;
	return S1;
};

TFile* sigFile = TFile::Open("tag24-08/HbbjjaSM125.root");
TFile* bkgFile = TFile::Open("tag24-08/NonResbbjja.root");
void evolution(int kfold=0){
	vector<Double_t> R;
	int size1=0;
	int size2=0;
	int size3=0;
	int size4=0;
	Double_t Rmax=0;
	int which=0;
	Double_t R_original=0;
	//numbering the variables
	vector<string> variables;
        variables.push_back("HT_soft");
	variables.push_back("dRB1Ph");
	variables.push_back("dRB2Ph");
	variables.push_back("dRJ1Ph");
	variables.push_back("dRJ2Ph");
	variables.push_back("pTBal");
	variables.push_back("WidthJ1");
	variables.push_back("WidthJ2");
        variables.push_back("cenPhJJ");
	variables.push_back("dPhiPhBmin");
	variables.push_back("dRB1J1");
	variables.push_back("dRB1J2");
	variables.push_back("dRB2J1");
	variables.push_back("dRB2J2");
	variables.push_back("dPhiJJ");
	variables.push_back("cenHJJ");
	variables.push_back("cenHgJJ");
	variables.push_back("pTBB");
	variables.push_back("pTPh");
	variables.push_back("cosThetaC");
	variables.push_back("cosThetaA");
	variables.push_back("pTB2");
	variables.push_back("pTJ1");
	variables.push_back("pTJ2");
	variables.push_back("dRJJ");
	variables.push_back("etaJ1");
	variables.push_back("etaJ2");
	variables.push_back("etaB1");
	variables.push_back("etaB2");
	variables.push_back("etaPh");
        //fixed
	variables.push_back("mJJ");
        variables.push_back("dEtaJJ");
        //spectator
	variables.push_back("mBB");


	//set and initialize every vector and number
	vector<int> combination1;
	vector<int> winner;
	vector<vector<int> > combination;
	vector<int> combination2; 
	vector<int> combination3;
	vector<int> combination4;
	vector<int> combination_tmp;
	//user's choice
	const int Iteration=50;
	double series[Iteration];
        const int halt=10;
	Double_t a=0.005;
	TTree* sig = (TTree*)sigFile->Get("Nominal");
	TTree* bkg = (TTree*)bkgFile->Get("Nominal");
	size1 = mvafactory(combination1, sig, bkg, 0, kfold );
	for(int in=0;in<Iteration;in++)
	{
               
		combination_tmp = combination1;
		if (in!=0) size1 = mvafactory(combination_tmp, sig, bkg, 1,  kfold);
		double S1 = mvasn();
		R.push_back(S1-a*size1);
		cout << "YJ interation "<< in << "R[0] = "<<R[0] << endl;
		combination1 = combination_tmp;
		combination_tmp = combination1;
		size2 = mvafactory(combination_tmp, sig, bkg, 2,  kfold);
		double S2 = mvasn();
		R.push_back(S2-a*size2);
		cout << "YJ interation "<< in << "R[1] = "<<R[1] << endl;
		combination2 = combination_tmp;
		combination_tmp = combination1;
		size3 = mvafactory(combination_tmp, sig, bkg, 3,  kfold);
		double S3 = mvasn();
		R.push_back(S3-a*size3);
		cout << "YJ interation "<< in << "R[2] = "<<R[2] << endl;
		combination3 = combination_tmp;
		combination_tmp = combination1;
		size4 = mvafactory(combination_tmp, sig, bkg, 4,  kfold);
		double S4 = mvasn();
		R.push_back(S4-a*size4);
		cout << "YJ interation "<< in << "R[3] = "<<R[3] << endl;
		combination4 = combination_tmp;
		Rmax=R[0]; 
		which=0;

		for(int i=1;i<4;i++)
		{
			if(Rmax<R[i])
			{
				Rmax=R[i];
				which=i;

			}
		}
		series[in] = Rmax;
		cout << "YJ interation "<< in << "Rmax = "<<Rmax << endl;
		cout << "...................YJ finish iteration " << in <<" .............." << endl;

		//find the corresponding combination
		if (which==0) winner = combination1;
		else if (which==1) winner = combination2;
		else if (which==2) winner = combination3;
		else if (which==3) winner = combination4;
		//winner=combination.at(which);
		for(int i=0;i<winner.size();i++)
		{
			cout<<"the winner is:   "<< winner.at(i)<< " " << variables.at(winner.at(i))<<endl;
		}
                        cout<<"the winner is:   "<< "30" <<" "<<variables.at(30)<<endl;
                        cout<<"the winner is:   "<< "31" <<" "<<variables.at(31)<<endl;
		for(int i=0; i< in+1; i++) cout <<"The "<< i <<" iteration Rmax: "<< series[i]<< endl;
                                
		if(in==0)
		{
			R_original=R[0];
		}
		//back to loop
		//combination.clear();
		combination1.clear();
		combination2.clear();
		combination3.clear();
		combination4.clear();
		combination_tmp.clear();
		R.clear();
		combination1=winner;
		size1=combination1.size();
		winner.clear();
                //run the winner
                
                if(in==Iteration-1)
                {
                      combination_tmp = combination1;
	       	      size1 = mvafactory(combination_tmp, sig, bkg, 1,  kfold);
                }
		cout<<"the number of variables is:   "<<size1+2<<endl;
		cout<<"the optimized R is:  "<<Rmax<<endl;
		cout<<"the optimized ratio is:   "<<Rmax+a*(size1+2)<<endl;
		cout<<"the R that we start with is:  "<<R_original<<endl;
		cout<<"the ratio that we start with is:   "<<R_original+a*11<<endl;
                //halt the program
                if(in>halt-1&&series[in]==series[in-halt+1])
                {
                       in=Iteration;
                }
                           
                
	}
};

