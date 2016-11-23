#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <chrono>
#include "dynamics.h"
#include "neuron.h"
#include "cerebellum.h"
#include "plant.h"



using namespace std;


#define NUMBER_OF_SIMULATION_STEPS 300000
#define LEARNING_RATE 1.0
#define LOG_DATA 1

int main(int argc, char *argv[])
{
        cout << endl << endl;
	cout<<"+++++++++++++++++++++++++++++++++"<<endl;
	cout<<"+++++++CerebellumSimple++++++++++"<<endl;
	cout<<"+++++++++++++++++++++++++++++++++"<<endl;

        cout << endl << endl;
   
#if LOG_DATA == 1
ofstream training_log, weights_log, finalweights_log, error_log;

        training_log.open("logTraining.dat");
        training_log.precision(8);
        training_log 
        << "time" <<            '\t'
        << "filtered Ref" <<    '\t'
        << "plant" <<           '\t'
        << "Error" <<           '\t'
        << "Control Signal" << endl;
        
        weights_log.open("logWeights.dat");
        weights_log 
        << "time" <<            '\t' 
        << "Weights-->" << endl;     
        
        finalweights_log.open("logWFinal.dat");
        finalweights_log
        << "index" <<           '\t'
        << "zero"  <<           '\t'
        << "W_init"<<           '\t' 
        << "W"     << endl;
        
        
        error_log.open("logError.dat");                 //
        error_log                                       //
        << "index" <<           '\t'                    //
        << "error" <<           '\t'                    //
        << "Abs error" <<       '\t'                    //
        << "avg error" <<       endl;                   //
#endif

        
        // Create Adaptive Filter
        Cerebellum FWD;
        
        string filenName("neuronDynamicsAlex10-100ms.dat");
        FWD.setNeuronalDynamics(filenName);
        
        string connectionfilenName("networkConnectionsAlex10-100ms.dat");
        FWD.setConnections(connectionfilenName);
	
        
        
#if LOG_DATA == 1
vector <double> w_init;
        for(int k = 0; k<FWD.neurons.size(); k++){
                w_init.push_back(FWD.outputWeights[k]);
        }
#endif
        
        
        // create Filter
        vector <double> asRef;
	vector <double> bsRef;
	asRef.clear();
	bsRef.clear();
	
                // Band-Pass Butterworth Filter ( 0.5-2.0 Hz )
                asRef.push_back( 1.0);
                asRef.push_back(-3.977786058262946);
                asRef.push_back( 5.933603493831731);
                asRef.push_back(-3.933847942486183);
                asRef.push_back( 0.9780305084587277);
                bsRef.push_back( 0.0);
                bsRef.push_back( 7.837379757436192e-05);
                bsRef.push_back(-7.895199724392678e-05);
                bsRef.push_back(-7.721739823523945e-05);
                bsRef.push_back( 7.779559790480432e-05);
                
        Neuron preFilter;	

        preFilter.setSize(1);
        preFilter.setWeight(0,1.0);
        preFilter.setInput(0,0.0);

        preFilter.setDynamics(bsRef,asRef);
        preFilter.setIC(0.0);
        
        
        // create plant
        Plant plant;
        string fileName("testDynamics.dat");
        plant.setDynamics(fileName);
        



        // Random Noise
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937 generator (seed);
        uniform_real_distribution<double> distribution (-1.0,1.0);
        
        
        
	

	
        double error = 0.0, avg_error = 0.0, Ref = 0.0, delayedReference;
        double filteredRef, plantOutput, modelOutput;
        for (int i = 0; i < 1000000; i++)
        {

                Ref = distribution(generator);
                
                preFilter.setInput(0, Ref);
                preFilter.runIteration();
                filteredRef = preFilter.getOutput();
                
                plant.setInput(filteredRef);
                plant.runIteration();
                plantOutput = plant.getOutput();
                
                FWD.setInput(filteredRef);
                FWD.updateNetwork();
                modelOutput = FWD.getSingleOutput();  
                
                error = plantOutput - modelOutput;
                FWD.updateOutputWeights(LEARNING_RATE,1.0,error);  
                               
#if LOG_DATA == 1
                
                training_log            
                << i                    << '\t'                   
                << preFilter.getOutput()<< '\t'
                << plant.getOutput()    << '\t'
                << error                << '\t'
                << FWD.getSingleOutput()    << endl;
                
                
                avg_error = avg_error*i/(i+1) +abs(error) * 1/(i+1);            //
                                                                                //
                error_log                                                       //
                << i            << '\t'                                         //
                << error        << '\t'                                         //
                << abs(error)   << '\t'                                         //
                << avg_error     << endl;                                       //
                
                weights_log << '\n' << i ;
                for (int j = 0; j < FWD.neurons.size() ; j++ ){
                        
                        weights_log << '\t' 
                        << FWD.outputWeights[j];
                                                
                } 
                

#endif


        }

        
#if LOG_DATA == 1
        for(int k = 0; k<FWD.neurons.size(); k++){
                finalweights_log
                << k <<         '\t'
                << 0.0 <<       '\t'
                << w_init[k] << '\t'
                << FWD.outputWeights[k] << endl;
        }
#endif
        
        

        
        
        
#if LOG_DATA == 1
        training_log.close();
        weights_log.close();
        finalweights_log.close();
#endif


        cout << "\n--------Simulation Complete--------" << endl << endl;
	return 1;
}
