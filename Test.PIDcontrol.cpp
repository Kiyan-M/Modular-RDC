#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "dynamics.h"
#include "neuron.h"
#include "cerebellum.h"
#include "fwd.h"
#include "TestBrain.cpp"



using namespace std;


#define NUMBER_OF_SIMULATION_STEPS 300000
#define LOG_DATA 1

int main(int argc, char *argv[])
{
        cout << endl << endl;
	cout<<"+++++++++++++++++++++++++++++++++"<<endl;
	cout<<"+++++++CerebellumSimple++++++++++"<<endl;
	cout<<"+++++++++++++++++++++++++++++++++"<<endl;

        cout << endl << endl;
   
#if LOG_DATA == 1
ofstream training_log, response_log;

        training_log.open("logTraining.dat");
        training_log.precision(8);
        training_log 
        << "time" << '\t' 
        << "reference" << '\t' 
        << "Plant" << '\t'
        << "Control Error" << '\t' 
        << "Control Signal" << endl;
   
        
        response_log.open("logResponse.dat");
        response_log
        << "time" << '\t'
        << "Reference" << '\t'
        << "ref est" << '\t'
        << "error" << endl;    
#endif

        // creates controller
        TestBrain brain;

        // Random Noise
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937 generator (seed);
        uniform_real_distribution<double> distribution (-1.0,1.0);
        
        // creates reference signal
        vector <double> asRef;
	vector <double> bsRef;
	asRef.clear();
	bsRef.clear();
		
		// 1/(s+16)
	        asRef.push_back(1.0);
	        asRef.push_back(-0.852143788966211);
	        bsRef.push_back(0.0);
	        bsRef.push_back(0.147856211033789);
                
            
                
        Neuron refSignal;

        refSignal.setSize(1);
        refSignal.setWeight(0,1.0);
        refSignal.setInput(0,1.0);

        refSignal.setDynamics(bsRef,asRef);
        refSignal.setIC(0.0);


	
        // creates Plant Dynamics
        vector <double> asPlant;
	vector <double> bsPlant;
	asPlant.clear();
	bsPlant.clear();

	
	//Creates Plant 6.25/(s^2+4s+6.25)
        asPlant.push_back(1.0);
        asPlant.push_back(-1.960176806047199);
        asPlant.push_back(0.960789439152323);
        
        bsPlant.push_back(0.0);	
        bsPlant.push_back(0.308358651000130e-3);                
        bsPlant.push_back(0.304274454124356e-3);
	        
        Neuron Plant;

        Plant.setSize(1);
        Plant.setWeight(0,1.0);

        Plant.setDynamics(bsPlant,asPlant);
        Plant.runIteration();
        
	double IC = 0.0;
	Plant.setIC(IC);
	

	
        double error = 0.0, cumError = 0.0, TEMPref = 0.0, delayedReference;
        for (int i = 0; i < 1000; i++)
        {

                TEMPref = 1.0;

        
                refSignal.setInput(0,TEMPref);
                refSignal.runIteration();
                
                brain.setRef(refSignal.getOutput());
                brain.runIteration();
                
                Plant.setInput(0,brain.getOutput());
                Plant.runIteration();
                
                error = Plant.getOutput() - refSignal.getOutput();
                brain.Error = error;
                
                
#if LOG_DATA == 1

                training_log 
                << i                    << '\t' 
                << refSignal.getOutput()<< '\t' 
                << Plant.getOutput()    << '\t'
                << error                << '\t'
                << brain.getOutput() << endl;
                
                
                response_log
                << i                    << '\t' 
                << refSignal.getOutput()<< '\t' 
                << Plant.getOutput()    << '\t'
                << error                << '\t'
                << brain.getOutput() << endl;
#endif

        }
        
        

        
        
        
#if LOG_DATA == 1
        training_log.close();
        response_log.close();
#endif


        cout << "\n--------Simulation Complete--------" << endl << endl;
	return 1;
}




