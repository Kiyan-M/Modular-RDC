#ifndef __CEREBELLUM__H
#define __CEREBELLUM__H

#include "neuron.h"
#include "dynamics.h"
#include <iostream>
#include <fstream>
using namespace std;
#define PRINT_DEBUG_INFO 0
class Cerebellum
{
public:
	Cerebellum();
	/*
	 * reads dynamics (bks and aks) from file
	*/
	void setNeuronalDynamics(string &fileName );

	/*
	*reads connections from file
	*/
	void setConnections(string &fileName);

	/*
	* updates all neuron inputs
	* by reading from each neuron input the output source
	* and then updates the input buffer of this neuron
	* with the appropriate output value from the preceeding neuron 
	* returns the weighted sum
	*/
	double updateNetwork();

	void setInput(double Input);
	double getOutput(vector<double> &allOutputs);
	double getLowPassOutput();
	void openOutputFile(string &OutputDataFileName, string &OutputWeightsFileName);
	void closeOutputFile();
	void writeOutputsToOutputFile();
	void writeOutputsToOutputFile(const vector<double> &allOutputs);
	void updateOutputWeights(double learningRate, double lambda, double error);
	void getOutputWeights (vector<double> &allWeights);


        //Kiyan's Functions
        double getSingleOutput();
        void printSingleOutput();
        //End
        
	vector <Neuron *> neurons;
	vector <double> output;
	vector <double> outputWeights;

private:
	double cerebellarInput;
	double singleOutput;
	double lowPassSingleOutput;
	Dynamics outputDynamics;
	
	DelayLine Delay;

	ofstream myOutputfile,myWeightFile;
	
};
#endif 
