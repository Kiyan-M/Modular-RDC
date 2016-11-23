#include "neuron.h"

Neuron::Neuron()
{
		membraneDynamics.reset();
		out=0.0;
}

int Neuron::setDynamics(vector<double> bk, vector<double> ak)
{

	if (bk.size()==ak.size())
	{
		membraneDynamics.setBkAk(bk,ak);
		return 1;
	}
	else
	{
		return -1;
	}
}

void Neuron::setSize(unsigned int numInputs)
{
	in.resize(numInputs);
	w.resize(numInputs);
	inputSource.resize(numInputs);
	for (unsigned int i=0;i<numInputs;i++)
	{
		w[i]=0;
		in[i]=0;
		inputSource[i]=NO_CONNECTION; //
	}
}
void Neuron::addInput(int inputSourceIndex)
{
	w.push_back(0.0);
	in.push_back(0.0);
	inputSource.push_back(inputSourceIndex);
}

int Neuron::getInputConnections(vector<int> & outputIndices)
{
	outputIndices=inputSource;
	return 1;
}

int Neuron::connect(int inputIndex, int inputSourceIndex)
{
	if (inputIndex<inputSource.size())
	{
		inputSource[inputIndex]=inputSourceIndex;
		return 1;
	}
	else
	{
		return -1;
	}
}

int Neuron::setInput(unsigned int index, double value)
{
	if (index<in.size())
	{
		in[index]= value;
		return 1;
	}
	else
	{
		return -1;
	}
}

void Neuron::setIC(double IC)
{
        membraneDynamics.setIC(IC);
}

int Neuron::setWeight(unsigned int index, double value)
{
	if (index<w.size())
	{
		w[index]=value;
		return 1;
	}
	else
	{
		return -1;
	}

}

int Neuron::getWeights(vector<double> &weights)
{
	weights=w;
	return 1;
}

void Neuron::getState(vector<double> &ins, vector<double> &outs)
{
        membraneDynamics.getState(ins,outs);
}

void Neuron::setState(vector<double> ins, vector<double> outs)
{
        membraneDynamics.setState(ins,outs);
}


void Neuron::runIteration()
{
	//lets calculate the weighted sum of inputs
	double sumOfInputs=0;
	
	for (unsigned int i=0; i<w.size();i++)
	{
		sumOfInputs=sumOfInputs+ w[i]*in[i];
		
	}

	//now we could run this through some non-linear function
		
		//***not implemented yet***//
	//if (sumOfInputs > 1){
	//sumOfInputs = 1;	
	//}
                
	//we run the internal dynamics
		membraneDynamics.setInput(sumOfInputs);

	//we could also run the output of the membrane through some non-linear function

		//***not implemented yet***//

	//and now have the neuron output
		out = membraneDynamics.getOutput();

}
double Neuron::getOutput()
{
	return out;
}
