#include "cerebellum.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory.h>
#include <random>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include "neuron.h"

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
void Cerebellum::setNeuronalDynamics(string &fileName )
{
	//read file with neuronal dynamics
        string line;
        ifstream myfile (fileName.c_str() );

  // Random Noise
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator (seed);
        uniform_real_distribution<double> distribution (-0.001,0.001);

  if (myfile.is_open())
  {
    bool isBk=true; //the first line is a bk parameter
	vector <double> aks;
	vector <double> bks;
	aks.clear();
	bks.clear();
	int paraCount=0;
	while ( myfile.good() )
        {
          getline (myfile,line);
        
	  //now we extract the numbers from the line
	  char *a=new char[line.size()+1];
	  a[line.size()]=0; //null terminate the string
	  memcpy(a,line.c_str(),line.size());
	  char *dummy;
	  int k=0;
	  float value;
	  dummy=strtok(a,"\t "); //get string until the next delimiter
	  while(dummy!=NULL)
	  {
		  sscanf(dummy,"%f", &value); //convert to float

		  k++;
		  if (isBk)
		  { 
//			  cout<<"  is bk Value"<<endl;
			  bks.push_back(value);
		  }
		  else
		  {
//			  cout<<"  is ak Value!"<<endl;
			  aks.push_back(value);
		  }
		  dummy=strtok(NULL,"\t ");
	  }
	  //end of line reached
	  isBk=!isBk;
	  

	  if (isBk)
	  {
		  //this means we have a full set of bks and aks for a neuron
		  //we can set the dynamics
		  Neuron *singleNeuron = new Neuron;
		  singleNeuron->setDynamics(bks,aks);

		  neurons.push_back(singleNeuron);
		  output.push_back(0.0); //add a cerebellar output
		  outputWeights.push_back(distribution(generator)) ; //set the output weight to 0

		  //clear bks and aks
		  aks.clear();
		  bks.clear();
		  

	  }
//	  cout<<"-------Parameter set "<< paraCount<<" stored!"<<endl;
	  paraCount++;

    }
    myfile.close();
  }
}

void Cerebellum::setConnections(string &fileName)
{
  string line;
  ifstream myfile (fileName.c_str());
  if (myfile.is_open())
  {

	while ( myfile.good() )
    {
      getline (myfile,line);
	  //now we extract the numbers from the line
	  char *a=new char[line.size()+1];
	  a[line.size()]=0; //null terminate the string
	  memcpy(a,line.c_str(),line.size());
	  char *dummy;
	  int k=0;
	  float value;
	  dummy=strtok(a,"\t "); //get string until the next delimiter
	  int isOutputNeuron=1; //the first integer always specifies the output neuron, the remaining the target neurons
	  int isTargetNeuron=1; //if not target neuron than it is weight
	  int outputNeuronIndex=-2;
	  int targetNeuronIndex=-1;
	  while(dummy!=NULL)
	  {
		  sscanf(dummy,"%f", &value); //convert to int
		  k++;
		  if (isOutputNeuron)
		  {
			  isOutputNeuron=0;
			  outputNeuronIndex=(int) value; //store the output neuron index
		  }
		  else
		  {
			  //now we have the target neuron index and we can build the connection table
			  if (isTargetNeuron)
			  {
				  isTargetNeuron=0;	
				  targetNeuronIndex=(int) value;
				  
				  if ((targetNeuronIndex< (int) neurons.size())&&(outputNeuronIndex< (int) neurons.size()))
				  {
					  //only if the target neuron actually exists
					  if (targetNeuronIndex!=NO_CONNECTION)
					  {
						neurons[targetNeuronIndex]->addInput(outputNeuronIndex);
					  }
				  }
				  else
				  {
					  cout<<"NETWORK CONNECTION ERROR WITH OUTPUT"<<outputNeuronIndex<<"CONNECTIONG TO NEURON "<<value<<" NETWORK SIZE IS  "<<neurons.size()<<endl;
				  }
			  }
			  else //is weight value
			  {

				  isTargetNeuron=1; //next time the value is target neuron again
				  vector<int> allInputConnections ;
				  if (targetNeuronIndex!=NO_CONNECTION)
				  {
					  int latestInputIndex;
					  neurons[targetNeuronIndex]->getInputConnections(allInputConnections);
					  latestInputIndex=allInputConnections.size() - 1; 
					  neurons[targetNeuronIndex]->setWeight(latestInputIndex, value); //sets the weight

				  }
			  }

		  }
		  
		  dummy=strtok(NULL,"\t ");

	  }
	} //all lines
  }//file is open
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

	//
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
