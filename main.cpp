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
ofstream training_log, weights_log, finalweights_log, outputs_log, error_log, forward_log;       //
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
                                                        //
                                                        //
        forward_log.open("logFWD.dat");                 //
#endif                                                  //
//======================================================//
        
        
        //cout << "    INITIALISING System Parametres..." << endl;
        
        
        
        Plant plant;
        //string fileName("testDynamics.dat");
        string fileName("plantDynamics.dat"); //Plants in rane 5->50 steps of 5 (1->9)
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

                /*
                // Band-Pass Butterworth Filter ( 0.5-10.0 Hz )
                asRef.push_back( 1.0);
                asRef.push_back(-3.906756644346583);
                asRef.push_back( 5.724450493341636);
                asRef.push_back(-3.728613601606459);
                asRef.push_back( 0.9109196897991230);
                bsRef.push_back( 0.0);
                bsRef.push_back( 1.913246851237629e-03);
                bsRef.push_back(-1.971836935925982e-03);
                bsRef.push_back(-1.796066681879949e-03);
                bsRef.push_back( 1.854656766568302e-03);
                */
                
        Neuron preFilter;	

        preFilter.setSize(1);
        preFilter.setWeight(0,1.0);
        preFilter.setInput(0,0.0);

        preFilter.setDynamics(bsRef,asRef);
        preFilter.setIC(0.0);


        
        
        
        // creates controller //S
        Brain brain(1);
        //brain.addCerebellum();
        //brain.addCerebellum();
        brain.Lambdas[0]=1.0;

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
        
	        
	
	
        int nSteps = 100000;
        //cout << "    STARTING Training Phase... (" << nSteps << " steps)" << endl;
        
        plant.choose(5);

        double TOTAL1 = 0.0;
        double TOTAL2 = 0.0;
        double RandomNoise = 0.0, Reference=0.0, brainOutput=0.0, plantOutput = 0.0; 
        double error = 0.0, avg_error = 0.0, rms_error = 0.0, delayedError = 0.0, sqrd_error =0.0;
        for (int i = 0; i < nSteps; i++)
        {
                //if (i%(nSteps/10)==nSteps/10-1){cout << 1+10*i/(nSteps+1) << ' ' << flush;}
                
                
                /*if (i == nSteps/4){
                        plant.choose(7);
                        //brain.Lambdas[0] = 0.0;
                        //brain.Lambdas[1] = 1.0;
                }
                else if(i == 2*nSteps/4){
                        plant.choose(1); //******************
                        //brain.Lambdas[0] = 1.0/2.0;
                        //brain.Lambdas[1] = 1.0/2.0;
                        for (int z = 0; z<500; z++){
                                //brain.RMS_FWDerror[0]->setLength(500);
                                //brain.RMS_FWDerror[1]->setLength(500);
                                //brain.RMS_FWDerror[0]->feed(0.0125);
                                //brain.RMS_FWDerror[1]->feed(0.0125);
                        }
                }
                else if(i == 3*nSteps/4){
                        plant.choose(7);
                }
                else if(i == 4*nSteps/5){
                        plant.choose(4);
                }*/
                
                
                
                RandomNoise = distribution(generator);
                
                preFilter.setInput(0,RandomNoise);
                preFilter.runIteration();
                Reference = preFilter.getOutput();
                
                brain.setRef(Reference);
                brain.runIteration();
                brainOutput = brain.getOutput();
                
                plant.setInput(brainOutput);
                //plant.setInput(Reference);
                plant.runIteration();
                plantOutput = plant.getOutput();
                
                brain.calculateErrors(plantOutput);


                error = brain.RefModel.getOutput() - plantOutput;
                
                        brain.updateWeights();
                        TOTAL1 += fabs(error);
                        TOTAL2 += fabs(brain.FWD[0]->getSingleOutput()-plantOutput);

                if (i > 2*nSteps/4){
                        brain.updateLambdas();
                }
                
                      

                
//==============================================================================//
#if LOG_DATA == 1                                                               //
                                                                                //
                training_log                                                    //
                << i                    << '\t'                                 //
                << Reference            << '\t'                                 //
                << brain.RefModel.getOutput() << '\t'                           //
                << plantOutput          << '\t'                                 //
                << error                << '\t'                                 //
                << brainOutput          << endl;                                //
                                                                                //
                weights_log << '\n' << i ;                                      //
                outputs_log << '\n' << i ;                                      //
                double sumOP = 0.0;                                             //
                for (int k = 0; k < brain.FWD[0]->neurons.size() ; k++ ){       //
                                                                                //
                                                                                //
                        outputs_log << '\t'                                     //
                        << brain.FWD[0]->output[k];                             //
                                                                                //
                        sumOP +=  brain.FWD[0]->output[k];                      //
                }                                                               //
                outputs_log << '\t' << brain.CB_output << '\t' << sumOP;        //
                                                                                //
                avg_error = avg_error*i/(i+1) +abs(error) * 1/(i+1);            //
                                                                                //
                error_log                                                       //
                << i            << '\t'                                         //
                << error        << '\t'                                         //
                << abs(error)   << endl;                                        //
                                                                                //
                forward_log                                                     //
                << i << '\t'                                                    //
                << plantOutput                     << '\t'; 
                
                double sumFWD = 0.0;
                for (int k = 0; k<brain.FWD.size() ;k++){                       //
                double FWDerror = fabs(brain.FWD[k]->getSingleOutput()-plantOutput);
                        forward_log 
                        << brain.FWD[k]->getSingleOutput() << '\t'           //
                        << brain.Lambdas[k] << '\t'
                        << FWDerror << '\t';
                        
                        sumFWD += brain.FWD[k]->getSingleOutput()*brain.Lambdas[k];
                        
                        
                        for (int j = 0; j < brain.FWD[0]->neurons.size() ; j++ ){       //
                                weights_log << '\t'                                     //
                                << brain.FWD[k]->outputWeights[j] << '\t';              //  
                                }                                                       //
                }
                
                forward_log << sumFWD << endl;
#endif                                                                          //
                                                                                //
//==============================================================================//
        }
        cout << "TOTAL CTRL ERROR : " << TOTAL1 << endl;
        cout << "TOTAL FWD ERROR : " << TOTAL2 << endl;
        

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

/*
        plant.choose(2);
        for (int j = 0; j < 100; j++){
        
                RandomNoise = distribution(generator);
                
                preFilter.setInput(0,RandomNoise);
                preFilter.runIteration();
                Reference = preFilter.getOutput();
                
                brain.setRef(Reference);
                brain.runIteration();
                brainOutput = brain.getOutput();
                
                plant.setInput(brainOutput);
                plant.runIteration();
                plantOutput = plant.getOutput();
                
                brain.updateLambdas();
                brain.printLambdas();
        }
*/
        
        
        
#if LOG_DATA == 1
        training_log.close();
        weights_log.close();
        finalweights_log.close();
        outputs_log.close();
        error_log.close();
        forward_log.close();
#endif


        //cout << "\nSIMULATION COMPLETE" << endl;
        //cout << "*******************************" << endl << endl << endl;
	return 1;
}
