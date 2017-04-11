#ifndef __BRAIN__H
#define __BRAIN__H  

#include "cerebellum.h"

#include <limits>


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
	vector <Cerebellum *> C;//  Cerebellar controllers
	vector <Cerebellum *> F;// Forward Plant Models
	
	
	double Reference;
	double C_output; 
	DelayLine DelayStem;
	double Motor_CMD;

        //Errors	
	double  Error;
        
        vector <double> F_error;
        vector <DelayLine *> RMS_Ferror;
        
	vector <double> Lambdas; //Responsibility estimators
        
private:


};

#endif
