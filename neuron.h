
#ifndef __NEURON__H
#define __NEURON__H
#include <iostream>
#include <vector>
#include "dynamics.h"
using namespace std;

#define NO_CONNECTION -2
#define INPUT_LAYER -1
#define OUTPUT_LAYER -3
#define FILTERED_OUTPUT_LAYER -4
class Neuron
{
public:
	Neuron();
    int setDynamics(vector<double> bk, vector<double> ak);
	/*

	*/
	void addInput(int inputSourceIndex); 
	void setSize(unsigned int numInputs);
	int setInput(unsigned int index, double value);
	void setInputs(vector<double> inputs);
	int connect(int inputIndex, int inputSourceIndex);
	void setIC(double IC);

	int setWeight(unsigned int index, double value);
	int setWeights(vector<double> weights);

	int getWeights(vector<double> &weights);

	int getInputConnections(vector<int> & outputIndices);
	
	void getState(vector<double> &ins, vector<double> &outs);
	void setState(vector<double> ins, vector<double> outs);

	void runIteration();
	double getOutput();
	Dynamics membraneDynamics;

//private:
	
	vector<double> w;
	vector<double> in;
	vector<int> inputSource;
	double out;
	
};


#endif 
