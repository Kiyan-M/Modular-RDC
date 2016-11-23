#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "dynamics.h"
#include "neuron.h"
#include "cerebellum.h"
#include "plant.h"
#include "brain.h"



using namespace std;


#define NUMBER_OF_SIMULATION_STEPS 300000
#define LOG_DATA 1
#define ECHO 1

int main(int argc, char *argv[])
{
//======================================================//
        cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^"  << endl;   //
	cout<<"STARTING Simulation..." << endl;         //
                                                        //
                                                        //
                                                        //
#if LOG_DATA == 1                                       //
ofstream training_log, weights_log, finalweights_log, outputs_log, error_log;       //
                                                        //
        training_log.open("logTraining.dat");           //
        training_log.precision(8);                      //
        training_log                                    //
        << "time" << '\t'                               //
        << "reference" << '\t'                          //
        << "RefModel " << '\t'                          //
        << "Response " << '\t'                          //
        << "Error    " << '\t'                          //
        << "Brain" << endl;                             //
                                                        //
        weights_log.open("logWeights.dat");             //
        weights_log                                     //
        << "time" << '\t'                               //
        << "Weights-->" << endl;                        //
                                                        //
        finalweights_log.open("logWFinal.dat");         //
        finalweights_log                                //
        << "index" <<           '\t'                    //
        << "zero"  <<           '\t'                    //
        << "W_init"<<           '\t'                    //
        << "W"     << endl;                             //
                                                        //        
        outputs_log.open("logOutputs.dat");             //
        outputs_log                                     //
        << "index" <<           '\t'                    //
        << "Outputs-->"     << endl;                    //
                                                        //        
        error_log.open("logError.dat");                 //
        error_log                                       //
        << "index" <<           '\t'                    //
        << "error" <<           '\t'                    //
        << "Abs error" <<       '\t'                    //
        << "avg error" <<       endl;                   //
#endif                                                  //
//======================================================//
        
        
        cout << "    INITIALISING System Parametres..." << endl;
        
        
        Plant plant;
        string fileName("testDynamics.dat");
        plant.setDynamics(fileName);
        
       // creates Low-pass Filter //
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


        
        
        
        // creates controller //S
        Brain brain;
        brain.addCerebellum();
        brain.Lambdas[0]=1.0;
        brain.printLambdas();

#if LOG_DATA == 1
        vector <double> w_init;
        for(int k = 0; k<brain.CB[0]->neurons.size(); k++){
                w_init.push_back(brain.CB[0]->outputWeights[k]);
        }
#endif


        // Random Noise //
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        mt19937 generator (seed);
        uniform_real_distribution<double> distribution (-1.0,1.0);
        
	        

	// Reference Model //
	        // creates RefModel Dynamics
        vector <double> asRefModel;
	vector <double> bsRefModel;
	asRefModel.clear();
	bsRefModel.clear();


        asRefModel.push_back(1.0);
        asRefModel.push_back(-0.9048374180359595);
        //asRefModel.push_back(-0.9813269859720434);
        //asRefModel.push_back(-0.9512);

        bsRefModel.push_back(0.0);
        bsRefModel.push_back(0.09513258196404044);
        //bsRefModel.push_back(1.867301402795667e-2); 
        //bsRefModel.push_back(0.0098);
        
	        
        Neuron RefModel;

        RefModel.setSize(1);
        RefModel.setWeight(0,1.0);

        RefModel.setDynamics(bsRefModel,asRefModel);
        RefModel.runIteration();
        
	RefModel.setIC(0.0);
	
	
        int nSteps = 100000;
        cout << "    STARTING Training Phase... (" << nSteps << " steps)" << endl;
        double error = 0.0, Ref = 0.0, Filtered_Ref=0.0, avg_error = 0.0;
        for (int i = 0; i < nSteps; i++)
        {
                
                if (i==50000){
                        plant.choose(0);
                }
                
                Ref = distribution(generator);
                
                preFilter.setInput(0,Ref);
                preFilter.runIteration();
                Filtered_Ref = preFilter.getOutput();
                
                brain.setRef(Filtered_Ref);
                brain.runIteration();
                cout << "main 177"<< endl;
                plant.setInput(brain.getOutput());
                plant.runIteration();
                
                cout << "main 181"<< endl;
                RefModel.setInput(0,Filtered_Ref);
                RefModel.runIteration();
                cout << "main 184"<< endl;
                
                error = RefModel.getOutput() - plant.getOutput();
                //error = Filtered_Ref - Plant.getOutput();
                brain.Error = error;
                
                
                if (i>-1)    {brain.updateWeights();}
                else            {}
                
//==============================================================================//
#if LOG_DATA == 1                                                               //
                                                                                //
                training_log                                                    //
                << i                    << '\t'                                 //
                << Filtered_Ref         << '\t'                                 //
                << RefModel.getOutput() << '\t'                                 //
                << plant.getOutput()    << '\t'                                 //
                << error                << '\t'                                 //
                << brain.getOutput()    << endl;                                //
                                                                                //
                weights_log << '\n' << i ;                                      //
                outputs_log << '\n' << i ;  
                double sumOP = 0.0;                                    //
                for (int j = 0; j < brain.CB[0]->neurons.size() ; j++ ){        //
                                                                                //
                        weights_log << '\t'                                     //
                        << brain.CB[0]->outputWeights[j];                       //
                                                                                //
                        outputs_log << '\t'                                     //
                        << brain.CB[0]->output[j];
                        
                        sumOP +=  brain.CB[0]->output[j];                       //
                }                                                               //
                outputs_log << '\t' << brain.CB_output << '\t' << sumOP;        //
                                                                                //
                avg_error = avg_error*i/(i+1) +abs(error) * 1/(i+1);            //
                                                                                //
                error_log                                                       //
                << i            << '\t'                                         //
                << error        << '\t'                                         //
                << abs(error)   << '\t'                                         //
                << avg_error     << endl;                                       //
#endif                                                                          //
                                                                                //
//==============================================================================//
        }
        cout << "    COMPLETED Training Phase" << endl;
        

#if LOG_DATA == 1
        for(int k = 0; k<brain.CB[0]->neurons.size(); k++){
                finalweights_log
                << k <<         '\t'
                << 0.0 <<       '\t'
                << w_init[k] << '\t'
                << brain.CB[0]->outputWeights[k] << '\t'
                << brain.CB[0]->outputWeights[k]-w_init[k] << endl;
        }
#endif

        
        
        
#if LOG_DATA == 1
        training_log.close();
        weights_log.close();
        finalweights_log.close();
        outputs_log.close();
        error_log.close();
#endif


        cout << "\nSIMULATION COMPLETE" << endl;
        cout << "*******************************" << endl << endl << endl;;
	return 1;
}


////////////////////////////////////////////
