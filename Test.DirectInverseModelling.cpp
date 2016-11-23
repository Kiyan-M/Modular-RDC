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
ofstream training_log, weights_log, response_log, finalweights_log;

        training_log.open("logTraining.dat");           //
        training_log.precision(8);                      //
        training_log                                    //
        << "time" << '\t'                               //
        << "reference" << '\t'                          //
        << "Response" << '\t'                           //
        << "Brain" << '\t'                              //
        << "Error" << endl;                             //
                                                        //
        weights_log.open("logWeights.dat");             //
        weights_log                                     //
        << "time" << '\t'                               //
        << "Weights-->" << endl;                        //
                                                        //        
        response_log.open("logResponse.dat");           //
        response_log                                    //
        << "time" << '\t'                               //
        << "Reference" << '\t'                          //
        << "Plant" << '\t'
        << "Brain"    << '\t'                           //
        << "Error" << endl;  
        
        finalweights_log.open("logWFinal.dat");
        finalweights_log
        << "index" <<           '\t'
        << "zero"  <<           '\t'
        << "W_init"<<           '\t' 
        << "W"     << endl;
#endif

        
        
        // creates controller
        TestBrain brain;
        
#if LOG_DATA == 1
        vector <double> w_init;
        for(int k = 0; k<brain.CB[0]->neurons.size(); k++){
                w_init.push_back(brain.CB[0]->outputWeights[k]);
        }
#endif

        
        // creates reference signal
        vector <double> asRef;
	vector <double> bsRef;
	asRef.clear();
	bsRef.clear();
		
		// Butterworth Low Pass at 100Hz
	        asRef.push_back(1.0);
	        asRef.push_back(-1.858806130513315);
	        asRef.push_back(0.868123445394585);
	        bsRef.push_back(0.0);
	        bsRef.push_back(0.004768463045194);
	        bsRef.push_back(0.004548851836076);
                
                
        Neuron LP_Filter;

        LP_Filter.setSize(1);
        LP_Filter.setWeight(0,1.0);
        LP_Filter.setInput(0,1.0);

        LP_Filter.setDynamics(bsRef,asRef);
        LP_Filter.setIC(0.0);


        // Random Noise
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937 generator (seed);
        uniform_real_distribution<double> distribution (-1.0,1.0);
        
        
        
        // creates Plant Dynamics
        vector <double> asPlant;
	vector <double> bsPlant;
	asPlant.clear();
	bsPlant.clear();

	/*
	//Creates Plant 6.25/(s^2+4s+6.25)
        asPlant.push_back(1.0);
        asPlant.push_back(-1.960176806047199);
        asPlant.push_back(0.960789439152323);
        
        bsPlant.push_back(0.0);	
        bsPlant.push_back(0.308358651000130e-3);                
        bsPlant.push_back(0.304274454124356e-3);
        */
        
	//2nd order plant w/ w0 = 40Hz zeta = 1.0, Critically Damped
        asPlant.push_back(1.0);
        asPlant.push_back(-1.921578878304646);
        asPlant.push_back(0.9231163463866355);
        
        bsPlant.push_back(0.0);	
        bsPlant.push_back(7.789832815838625e-04);                
        bsPlant.push_back(7.584848004055026e-04);
	        
        Neuron Plant;

        Plant.setSize(1);
        Plant.setWeight(0,1.0);

        Plant.setDynamics(bsPlant,asPlant);
        Plant.runIteration();
        
	double IC = 0.0;
	Plant.setIC(IC);
	

	
        double error = 0.0, Ref = 0.0;
        for (int i = 0; i < 1000000; i++)
        {

                Ref = distribution(generator);

        
                LP_Filter.setInput(0,Ref);
                LP_Filter.runIteration();
                
                Plant.setInput(0,LP_Filter.getOutput());
                Plant.runIteration();
                
                brain.setRef(Plant.getOutput());
                brain.runIteration();
                
                
                
                error =  LP_Filter.getOutput() - brain.getOutput();
                brain.Error = error;
                
                
                brain.updateWeights();
                
#if LOG_DATA == 1

                training_log 
                << i                    << '\t' 
                << LP_Filter.getOutput()<< '\t' 
                << Plant.getOutput()    << '\t'
                << brain.getOutput()    << '\t'
                << error                << endl;
                
                weights_log << '\n' << i ;
                for (int j = 0; j < brain.CB[0]->neurons.size() ; j++ ){
                        
                        weights_log << '\t' 
                        << brain.CB[0]->outputWeights[j];
                                                
                } 
                
#endif
        }
        
        LP_Filter.setIC(0.0);
        Plant.setIC(0.0);
        for (int j = 0; j < 500; j++){
                Ref = 1.0;
                
                LP_Filter.setInput(0,Ref);
                LP_Filter.runIteration();
                
                Plant.setInput(0,LP_Filter.getOutput());
                Plant.runIteration();
                
                brain.setRef(Plant.getOutput());
                brain.runIteration();
                
                error = fabs(brain.getOutput() - LP_Filter.getOutput());
                
#if LOG_DATA ==1
                response_log
                << j                    << '\t'
                << LP_Filter.getOutput()<< '\t' 
                << Plant.getOutput()    << '\t'
                << brain.getOutput()    << '\t'
                << error                <<endl;
#endif
                
        }

        
#if LOG_DATA == 1
        for(int k = 0; k<brain.CB[0]->neurons.size(); k++){
                finalweights_log
                << k <<         '\t'
                << 0.0 <<       '\t'
                << w_init[k] << '\t'
                << brain.CB[0]->outputWeights[k] << endl;
        }
#endif        
        
#if LOG_DATA == 1
        training_log.close();
        weights_log.close();
        response_log.close();
        finalweights_log.close();
#endif


        cout << "\n--------Simulation Complete--------" << endl << endl;
	return 1;
}




