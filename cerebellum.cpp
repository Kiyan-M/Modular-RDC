#include "cerebellum.h"

using namespace std;

 Cerebellum::Cerebellum()
{
	output.clear(); //clears all the otuptuts
	outputWeights.clear();
	singleOutput=0.0;
	lowPassSingleOutput=0.0;

	vector<double> outputBk, outputAk;

	//filter with infinite cut-off, no phase shift (basically no filter)
	outputBk.push_back(1.0); outputBk.push_back(0.0);  outputBk.push_back(0.0);
	outputAk.push_back(1.0); outputAk.push_back(0.0); outputAk.push_back(0.0);

	outputDynamics.setBkAk(outputBk,outputAk);
	outputDynamics.reset();
	
	
	
        // DelayLine
        Delay.clear();
        Delay.setLength(100);
        
        
}
void Cerebellum::setNeuronalDynamics(double aMin, double aMax, int aCount )
{

        bool log_spacing = 1;

        double T=0.001;        
        assert(aMax>=aMin);
        assert(aCount>0);
        
        double d_a = (aMax-aMin)/(double(aCount-1));
        


        // Random Noise
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator (seed);
        uniform_real_distribution<double> distribution (-0.001,0.001);
        
	vector <double> aks;
	vector <double> bks;
	aks.clear();
	bks.clear();
 
        
        // First Neuron has constant input 1.0
        bks.push_back(0.0);
        bks.push_back(1.0);
        aks.push_back(1.0);
        aks.push_back(0.0);
        
        Neuron *singleNeuron = new Neuron;
	singleNeuron->setDynamics(bks,aks);

	singleNeuron->addInput(-5);
	singleNeuron->setWeight(0,1.0);
        
	neurons.push_back(singleNeuron);
	output.push_back(0.0); //add a cerebellar output
	outputWeights.push_back(0.0);
        
        aks.clear();
        bks.clear();
        
        
        if (log_spacing == 1){
                double aMin_log=log(aMin);
                double aMax_log=log(aMax);
                double d_a_log=(aMax_log-aMin_log)/(double(aCount-1));
                double a_log = aMin_log;
                
                double a = exp(a_log);
                for (int i=0; i< aCount; i++){
                        bks.push_back(0);
                        bks.push_back(a*a*T*T*exp(-a*T));
                        bks.push_back(0);
                        aks.push_back(1);
                        aks.push_back(-2*exp(-a*T));
                        aks.push_back( exp(-2*a*T));
                        
                        Neuron *singleNeuron = new Neuron;
		        singleNeuron->setDynamics(bks,aks);

		        singleNeuron->addInput(-1);
		        singleNeuron->setWeight(0,1.0);
                        
		        neurons.push_back(singleNeuron);
		        output.push_back(0.0); //add a cerebellar output
		        outputWeights.push_back(distribution(generator));
                        
                        aks.clear();
                        bks.clear();
                        //a +=d_a;
                        a_log+=d_a_log;
                        a=exp(a_log);
                }
        }
        else{
                
                double a = aMin;
                for (int i=0; i< aCount; i++){
                        bks.push_back(0);
                        bks.push_back(a*a*T*T*exp(-a*T));
                        bks.push_back(0);
                        aks.push_back(1);
                        aks.push_back(-2*exp(-a*T));
                        aks.push_back( exp(-2*a*T));
                        
                        Neuron *singleNeuron = new Neuron;
		        singleNeuron->setDynamics(bks,aks);

		        singleNeuron->addInput(-1);
		        singleNeuron->setWeight(0,1.0);
                        
		        neurons.push_back(singleNeuron);
		        output.push_back(0.0); //add a cerebellar output
		        outputWeights.push_back(distribution(generator));
                        
                        aks.clear();
                        bks.clear();
                        
                        a +=d_a;
                }
        }
}

void Cerebellum::setConnections()
{

}

double Cerebellum::updateNetwork()
{
	vector <int> outputIndices;
	
	for (int i=0; i<neurons.size(); i++)
	{
		//go through all neurons

		//find out which input receives which output

		outputIndices.clear();
		neurons[i]->getInputConnections(outputIndices);
		
		for (int k=0; k<outputIndices.size();k++)
		{
#if PRINT_DEBUG_INFO==1
			cout<<"Neuron: "<<i<<"  Input: "<<k<< "  receives Output: " << outputIndices[k];
#endif
			if (outputIndices[k]==NO_CONNECTION)
			{
				neurons[i]->setInput(k, 0.0); //set the input to 0
#if PRINT_DEBUG_INFO==1
				cout<<"  value: 0.0, no connection";
#endif
			} 
			else if (outputIndices[k]==INPUT_LAYER)
			{
				neurons[i]->setInput(k, cerebellarInput); 
#if PRINT_DEBUG_INFO==1
				cout<<"  value: cerebellarInput";
#endif 
			}

			else if (outputIndices[k]==OUTPUT_LAYER)
			{
				neurons[i]->setInput(k, singleOutput);
#if PRINT_DEBUG_INFO==1
				cout<<"  value: cerebellarOutput";
#endif
			}
			else if (outputIndices[k]==FILTERED_OUTPUT_LAYER)
			{
				neurons[i]->setInput(k, lowPassSingleOutput);
#if PRINT_DEBUG_INFO==1
				cout<<"  value: filtered cerebellarOutput";
#endif

			}
else if (outputIndices[k]==CONSTANT_UNIT_INPUT)
			{
				neurons[i]->setInput(k, 0.0001);
#if PRINT_DEBUG_INFO==1
				cout<<"  value: Constant value of 0.0001";
#endif

			}
			else //normal neuron connection 
			{
				//but only if this index is possible
				if (outputIndices[k]<neurons.size())
				{
					double neuronOutput=neurons[outputIndices[k]]->getOutput();
					neurons[i]->setInput(k,neuronOutput);
#if PRINT_DEBUG_INFO==1
					cout<<"  value:  "<< neuronOutput<< " (Neuron Output)";
#endif 

				}
				else
				{
					neurons[i]->setInput(k,0.0);
					cout<<"  value: 0, wrong connection";
				}
			}
#if PRINT_DEBUG_INFO==1
			cout<<endl;
#endif 
			
		}//through all inputs
	}//through all neurons

	//now that the neuron inputs have been set, we go through the neurons again and update their output state
#if PRINT_DEBUG_INFO==1
	cout<<endl<<endl<<"Output state"<<endl;
#endif

	singleOutput=0.0; // clear the single cerebellar output
	for (int i=0; i<neurons.size(); i++)
	{
		neurons[i]->runIteration();
		output[i]=neurons[i]->getOutput();
		//sum up the neuronal outputs to a weighted sum 
		singleOutput=output[i]*outputWeights[i] + singleOutput; // 
#if PRINT_DEBUG_INFO==1
	double lowPassSingleOutput;
		cout<<"Neuron "<<i<< "  has output value "<< output[i]<<endl;
#endif
	}
	//now we run the sum through the output filter

	outputDynamics.setInput(singleOutput);
	lowPassSingleOutput=outputDynamics.getOutput();

	return singleOutput;
}

void Cerebellum::setInput(double Input)
{
	cerebellarInput=Input;
}

double Cerebellum::getOutput(vector<double> &allOutputs)
{
	allOutputs=output;
	return singleOutput;

}

double Cerebellum::getLowPassOutput()
{
	return lowPassSingleOutput;
}


void Cerebellum::getOutputWeights (vector<double> &allWeights)
{
	allWeights=outputWeights;
}



void Cerebellum::openOutputFile(string &OutputDataFileName, string &OutputWeightsFileName)
{
	myOutputfile.open (OutputDataFileName.c_str());
	myWeightFile.open (OutputWeightsFileName.c_str());
}
void Cerebellum::closeOutputFile()
{
	myOutputfile.close();
	myWeightFile.close();
}
void Cerebellum::writeOutputsToOutputFile()
{
	for (int i=0;i<output.size();i++)
	{
		myOutputfile<<output[i]<<" ";
		myWeightFile<<outputWeights[i]<<" ";
	}
	myOutputfile<<endl;
	myWeightFile<<endl;
}

void Cerebellum::writeOutputsToOutputFile(const vector<double> &allOtherData)
{
	for (int i=0; i<allOtherData.size();i++)
	{
		myOutputfile<<allOtherData[i]<<" ";
	}
	for (int i=0;i<output.size();i++)
	{
		myOutputfile<<output[i]<<" ";
		myWeightFile<<outputWeights[i]<<" ";
	}
	myOutputfile<<endl;
	myWeightFile<<endl;
}


void Cerebellum::updateOutputWeights(double learningRate, double lambda, double error)
{
	
	for (int i=0; i<outputWeights.size(); i++)
	{
	        double delta_W = (error * output[i] * learningRate * lambda);
		outputWeights[i] = outputWeights[i] + delta_W;

	}
}





//Kiyan's functions
double Cerebellum::getSingleOutput()
{
        double output;
        
        output = singleOutput;
        //output = tanh(singleOutput);
        return output;
}

void Cerebellum::printSingleOutput()
{
        //cout << "Single Output : " << singleOutput << endl;
        cout << '\t' << singleOutput << endl;
}
