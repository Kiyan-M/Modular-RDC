#ifndef __BRAIN__H
#define __BRAIN__H  

#include "dynamics.h"
#include "neuron.h"
#include "cerebellum.h"
#include <math.h>
#include <limits>
using namespace std;


class Brain {

public:
	Brain(int numCerebellum = 1);
	int addCerebellum(int numCerebellum = 1);
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
	vector <Cerebellum *> CB;//  Cerebellar controllers
	vector <Cerebellum *> FWD;// Forward Plant Models
	
	
	double Reference;
	double CB_output; 
	DelayLine DelayStem;
	double Motor_CMD;

        //Errors	
	double  Error;
        
        vector <double> FWD_error;
        vector <RMS *> RMS_FWDerror;
        
	vector <double> Lambdas; //Responsibility estimators
        
private:


};

#endif
