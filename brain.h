#ifndef __BRAIN__H
#define __BRAIN__H  

#include "dynamics.h"
#include "neuron.h"
#include "cerebellum.h"
#include <math.h>
using namespace std;


class Brain {

public:
	Brain(int numCerebellum = 1);
	int addCerebellum();
	int runIteration();
	int updateWeights();
	void calculateErrors(double plantOutput);
	
	
	int updateLambdas();
	int setLambdas(vector <double> lambdas);
	int getLambdas(vector<double> &lambdas);
	int printLambdas();
	
	int setRef(double reference);
	double getRef();
        int printRef();
        
        double getOutput();
	
        Neuron RefModel;
	Neuron Brainstem;
	vector <Cerebellum *> CB;//  Cerebellar controllers (Inverse Models)
	vector <Cerebellum *> FWD;
	
	
	double Reference;
	double Filtered_Reference;
	double CB_output; 
	double Motor_CMD;
	double Error;
	DelayLine DelayStem;
        
        vector <double> FWD_error;
	vector <double> Lambdas;			// Responsibility estimators
	//int switch_type;

private:


};

#endif
