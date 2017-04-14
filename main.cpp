#include "main.h"


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
ofstream training_log, weights_log, finalweights_log, outputs_log, error_log, forward_log, temp_log;       //
                                         
        temp_log.open("logTemp.dat");
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
        forward_log.open("logFWD.dat");                 //
        forward_log                                     //
        << "index" <<           '\t'                    //
        << "Plant" <<           '\t'                    //
        << "FModel1" <<       '\t'                      //
        << "FError1" <<       '\t'                      //
        << "Lambda1" <<       endl;                     //
                                                        //
                                                        //
#endif                                                  //
//======================================================//
        
        
        
    PlantRK plant;
    plant.setDynamics(10.0);
        
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
    brain.Lambdas[0]=1.0;
#if LOG_DATA == 1
        vector <double> w_init;
        for(int k = 0; k<brain.C[0]->neurons.size(); k++){
                w_init.push_back(brain.C[0]->outputWeights[k]);
        }
#endif

    // Random Noise //
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 generator (seed);
    uniform_real_distribution<double> distribution (-1.0,1.0);
    
        
    int nSteps = 80000;
    //cout << "    STARTING Training Phase... (" << nSteps << " steps)" << endl;

    double RandomNoise = 0.0, Reference=0.0, brainOutput=0.0, plantOutput = 0.0,plantOGOutput = 0.0; 
    double error = 0.0, avg_error = 0.0, rms_error = 0.0, delayedError = 0.0, sqrd_error =0.0;
    for (int i = 0; i < nSteps; i++)
    {
            //if (i%(nSteps/10)==nSteps/10-1){cout << 1+10*i/(nSteps+1) << ' ' << flush;}
            
            
            /*if (i == floor(nSteps/4)){
                    plant.setDynamics(30.0);
                    brain.Lambdas[0] = 0.0;
                    brain.Lambdas[1] = 1.0;
            }
            else if(i == floor(2*nSteps/4)){
                    plant.setDynamics(10.0); //******************
                    brain.Lambdas[0] = 0.5;
                    brain.Lambdas[1] = 0.5;
                    }
            else if(i == floor(3*nSteps/4)){
                    plant.setDynamics(30.0);
            }*/
            
            //if(i%40000==0){plant.setDynamics(40.0);}
            //else if(i%20000==0){plant.setDynamics(10.0);}
            
            
            RandomNoise = distribution(generator);
            
            preFilter.setInput(0,RandomNoise);
            preFilter.runIteration();
            Reference = preFilter.getOutput();
            
            brain.setRef(Reference);
            brain.runIteration();
            brainOutput = brain.getOutput();
            
            plant.setInput(brainOutput);
            plant.runIteration(10);
            plantOutput = plant.getOutput();
            
            brain.calculateErrors(plantOutput);


            error = brain.RefModel.getOutput() - plantOutput;
            
            brain.updateWeights();
            brain.updateLambdas();

            

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
                for (int k = 0; k < brain.F[0]->neurons.size() ; k++ ){         //
                                                                                //
                                                                                //
                        outputs_log << '\t'                                     //
                        << brain.F[0]->output[k];                               //
                                                                                //
                        sumOP +=  brain.F[0]->output[k];                        //
                }                                                               //
                outputs_log << '\t' << brain.C_output << '\t' << sumOP;         //
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
                
                double sumF = 0.0;
                for (int k = 0; k<brain.F.size() ;k++){                       //
                double Ferror = fabs(brain.F[k]->getSingleOutput()-plantOutput);
                        forward_log 
                        << brain.F[k]->getSingleOutput() << '\t'           //
                        << Ferror << '\t'
                        << brain.Lambdas[k] << '\t';
                        
                        sumF += brain.F[k]->getSingleOutput()*brain.Lambdas[k];
                        
                        
                        for (int j = 0; j < brain.F[0]->neurons.size() ; j++ ){       //
                                weights_log << '\t'                                     //
                                << brain.F[k]->outputWeights[j] << '\t';              //  
                                }                                                       //
                }
                forward_log << endl;
                
                
#endif                                                                          //
                                                                                //
//==============================================================================//
        }

        

#if LOG_DATA == 1
        for(int k = 0; k<brain.C[0]->neurons.size(); k++){
                finalweights_log
                << k <<         '\t'
                << 0.0 <<       '\t'
                << w_init[k] << '\t'
                << brain.C[0]->outputWeights[k] << '\t'
                << brain.C[0]->outputWeights[k]-w_init[k] << endl;
        }
#endif


        
#if LOG_DATA == 1
        temp_log.close();
        training_log.close();
        weights_log.close();
        finalweights_log.close();
        outputs_log.close();
        error_log.close();
        forward_log.close();
#endif


	return 1;
}
