
/**
*
@ingroup myo_module
\defgroup cerebellumSimple cerebellumSimple

This software module implements a cerebellum inspired adaptive controller

\section portsc_sec Interface: Ports Created and data format


Output ports:

- /cerebellumSimple/output:o

Input ports:
- /cerebellumSimple/input:i

\section in_files_sec Input Data Files
The module does not read an input data file.

\section out_data_sec Output Data Files
The module does not write to an output data file.
 
\section conf_file_sec Configuration Files
- config.ini


\section tested_os_sec Tested OS

Windows only,so far. 

\section example_sec Example Instantiation of the Module


\author Alexander Lenz

Copyright: Alexander Lenz, BRL

This file can be edited at src/bert2MotorServer/main.cpp
**/

//Yarp includes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include <yarp/os/all.h>
//#include <stdio.h>
#include <iostream>



//#include "IntersectionPlane.h"
//#include "gazeReceiver.h"
//#include "dbAccess.h"

#include "dynamics.h"
#include "cerebellum.h"


using namespace std;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;








#define NUMBER_OF_SIMULATION_STEPS 300000
#define LEARNING_RATE 0.01
int main(int argc, char *argv[])
{
	cout<<"+++++++++++++++++++++++++++++++++"<<endl;
	cout<<"+++++++CerebellumSimple++++++++++"<<endl;
	cout<<"+++++++++++++++++++++++++++++++++"<<endl;


	Network yarp;
	Dynamics plant, headVelocityFilter, modelReference, inverseModelReference;
	//bert2MotorServer bert2; //
	//bert2.start();
	


	
	vector <double> as;
	vector <double> bs;
	as.clear();
	bs.clear();


	//parameters for plant dynamics of differentiating plant

	//ak: 1	-1.00000000000000
	//bk: 1	-0.990049833749168


	//parameters for plant dynamics of integrating plant

	//a0=1.000000, a1=-0.990050
	 //b0=0.000000, b1=0.009950


	as.push_back(1);
	as.push_back(-0.990050);

	bs.push_back(0.0);
	bs.push_back(0.009950);

	DelayLine testDelay;



  int dummy;
  dummy=1;



  plant.setAk(as);
  plant.setBk(bs);
  plant.reset();

  as.clear();
  bs.clear();


  //set up reference model


  /*
   * a0= 1
	 a1=-0.9048374180359595

	 b0=0
	 b1=0.09516258196404044
   */

 as.push_back(1);
 //as.push_back(-0.9048374180359595 );
 as.push_back(-0.9512);

 bs.push_back(0.0);
 //bs.push_back(0.09516258196404044);
  bs.push_back( 0.0098);


  modelReference.setAk(as);
  modelReference.setBk(bs);
  modelReference.reset();

  as.clear();
  bs.clear();



  //set up inverse of reference model

 // 9.99500083333289
 // -8.99500083333289
  //implements (s+100)/100  = 1 + s/100 in 'matched' c2d transformation
 bs.push_back(9.99500083333289);
 bs.push_back(-8.99500083333289);
 as.push_back(1);
 as.push_back(0);



 inverseModelReference.setAk(as);
 inverseModelReference.setBk(bs);
 inverseModelReference.reset();

  as.clear();
  bs.clear();

  //filter for random data, 2Hz, 2nd order low pass (butter) in 1000Hz system
  bs.push_back(  3.91302053991405e-05);as.push_back(1.0);
  bs.push_back( 7.82604107982809e-05);as.push_back(-1.98222892979253);
  bs.push_back( 3.91302053991405e-05);as.push_back(0.982385450614126);



  //filter for random data, 20Hz, 2nd order low pass (butter) in 1000Hz system
  //bs.push_back(0.003621681515);as.push_back(1);
  //bs.push_back(0.00724336303);as.push_back(-1.822694925);
  //bs.push_back(0.003621681515);as.push_back(0.8371816513);



  //filter random data 50Hz, 2nd order low pass (butter) in 1000Hz system

 // bs.push_back(0.02008336556);as.push_back(1);
 // bs.push_back(0.04016673113);as.push_back(-1.561018076);
  //bs.push_back(0.02008336556);as.push_back(0.6413515381);

  headVelocityFilter.setAk(as);
  headVelocityFilter.setBk(bs);
  headVelocityFilter.reset(); //filter ready to go
  double myIn=1; //set an input

  vector <double> outHistory;
  outHistory.clear();

  for (int i=0; i<500; i++)
  {
	  plant.setInput(myIn);
	  cout<<plant.getOutput()<<"  ";
	  outHistory.push_back(plant.getOutput());
	 // myIn=0.0;
  }

  plant.printParameters();


  cout<<endl<<endl<<endl;
  Cerebellum cere;

  //
  string filenName("neuronDynamicsCere1.dat");
  cere.setNeuronalDynamics(filenName);

  string connectionfilenName("networkConnectionsCere1.dat");
  cere.setConnections(connectionfilenName);

  int k=cere.neurons.size();
  cout<<"Number of Neurons: "<< cere.neurons.size()<<endl;

  for (int i=0;i<k;i++)
  {
	  cere.neurons[i]->membraneDynamics.printParameters();
  }

  cere.setInput(1.0) ; //set an input of 1.0

  //check adding inputs to a neuron
  //cere.neurons[0]->setSize(5);
  //cere.neurons[0]->connect(3, 99);

  //cere.neurons[0]->addInput(5);
  //cere.neurons[0]->addInput(6);

  //cere.neurons[5]->addInput(500);
  //cere.neurons[5]->addInput(501);

  //cere.neurons[5]->addInput(INPUT_LAYER);
  //cere.neurons[5]->addInput(NO_CONNECTION);
  vector<int> connections;

  cere.neurons[0]->getInputConnections(connections);



  cout<<"---------------"<<endl;
  cout<<"print connections of neuron 0"<<endl<<endl;
  for (int i=0;i<connections.size();i++)
  {
	  cout<<connections[i]<<endl;
	  //cere.neurons[0]->setWeight(i,i);
	  //cere.neurons[0]->setInput(i,10*i);
  }

  //cere.neurons[0]->runIteration();

  cout<<endl<<endl<<"UPDATE NETWORK"<<endl<<endl;

  vector <double> cerebellarOutputs;

  string outputFileName("outputDataCere1.txt");
  string outputWeightFileName("outputDataWeightsCere1.txt");
  cere.openOutputFile(outputFileName,outputWeightFileName);

  //simulate plant 
  float randomInput;
  vector <double> loggingData;
  loggingData.clear();
  loggingData.push_back(0); //filtered velocity signal
  loggingData.push_back(0); //brainstem output
  loggingData.push_back(0); //plant output;
  loggingData.push_back(0); //sensory Error;
  plant.reset();
  modelReference.reset();

  //Brainstem is modelled as a simple delay
  DelayLine Brainstem;
  Brainstem.setLength(2); //simple delay of two timesteps

  double brainstemOutput;
  double sensoryError;

  //seed the random number generator

  srand(55); 

  double referenceSignal; //
 
  int runStepInputOnly=0;
  double learningRate=LEARNING_RATE;


  int impulseInput=1;
  cout<<endl<<"STARTING SIMULATION!"<<endl;
  for (int i=0;i<NUMBER_OF_SIMULATION_STEPS;i++)
  {
    //cout<<" "<<i<<"  ";
	
	randomInput=(1.0*rand()/RAND_MAX)-0.5;
	headVelocityFilter.setInput(randomInput);

	referenceSignal=headVelocityFilter.getOutput();
	if (runStepInputOnly)
	{
		referenceSignal=impulseInput;
	}
	impulseInput=0; //an impulse is only 1 once in a run

	//the head velocity goes into the Brainstem 
	//together with the cerebellarOutput
	brainstemOutput = Brainstem.feed(cere.getOutput(cerebellarOutputs)+referenceSignal);

	//the brainstem output goes into the plant
	plant.setInput(brainstemOutput);

	//the same signal (the efferent copy is fed back into the cerbellum
	cere.setInput(brainstemOutput);
	//we update the cerebellum
	cere.updateNetwork();

	//we calculate the the plant reference model
		modelReference.setInput(referenceSignal);

	//we now calculate the error 
	sensoryError=plant.getOutput() - modelReference.getOutput(); // use the plant reference or the direct desired input i.e variable referenceSignal;

	//back-propagate then sensory error through the inverse of the model.
	inverseModelReference.setInput(sensoryError);

	//and train the cerebellum with error
	//cere.updateOutputWeights(learningRate, sensoryError);

	//and train the cerebellum with error backpropagted through inverse reference model
	cere.updateOutputWeights(learningRate, inverseModelReference.getOutput());

	//log some data
	loggingData[0]= modelReference.getOutput();//referenceSignal; //filtered velocity signal
	loggingData[1]= brainstemOutput;//brainstem output
	loggingData[2]= plant.getOutput();//plant output;
	loggingData[3]= sensoryError;//sensory Error;
	


	cere.writeOutputsToOutputFile(loggingData);
	cere.getOutput(cerebellarOutputs);

	//for (int k=0; k<cerebellarOutputs.size();k++)
	//{
	//	//cout<<cerebellarOutputs[k]<<"  ";
	//}
	//cout<<endl;
  }
  cere.closeOutputFile();
    cout<<endl<<"SIMULATION ENDED!"<<endl;




  while(dummy!=0)
  {
		cout<<endl<<endl<<"+++++++++++++++++++++++++++++++++"<<endl;
	    cout<<"Type '0' and press return to end server"<<endl;
		cout<<"+++++++++++++++++++++++++++++++++"<<endl;
		cin>>dummy;
  }
  //bert2.stop();
  return 1;
}




