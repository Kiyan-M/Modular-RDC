#include "brain.h"

#define LEARNING_RATE 0.01

Brain::Brain(int numCerebellum)
{
        Lambdas.clear();
        CB.clear();
        FWD.clear();

        // Create First FF Controller
        addCerebellum(numCerebellum);

        
        // creates Brainstem controller
        
        vector <double> asBrainstem;
	vector <double> bsBrainstem;
	asBrainstem.clear();
	bsBrainstem.clear();
                
                //Unity
                /*
                asBrainstem.push_back(1.0);
                bsBrainstem.push_back(1.0);
                */
                asBrainstem.push_back(1.0);
                asBrainstem.push_back(-0.9990004998333750);                
                bsBrainstem.push_back(1.05);                
                bsBrainstem.push_back(-1.0464517744084810);

                

        Brainstem.setSize(1);
        Brainstem.setWeight(0,1.0);
        Brainstem.setInput(0,1.0);

        Brainstem.setDynamics(bsBrainstem,asBrainstem);
        Brainstem.setIC(0.0);
        
        // DelayLine
        DelayStem.clear();
        DelayStem.setLength(2);

        
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
        
	
        RefModel.setSize(1);
        RefModel.setWeight(0,1.0);

        RefModel.setDynamics(bsRefModel,asRefModel);
        RefModel.runIteration();
        
	RefModel.setIC(0.0);
	
        
       
        
}

int Brain::addCerebellum(int numCerebellum)
{

        for (int i = 0; i<numCerebellum; i++){
                Cerebellum *newCerebellum = new Cerebellum;
                
                string filenName("neuronDynamicsL1-50ms.dat");
                newCerebellum->setNeuronalDynamics(filenName);
                string connectionfilenName("networkConnectionsL1-50ms.dat");
                newCerebellum->setConnections(connectionfilenName);
	
                CB.push_back(newCerebellum);
                Lambdas.push_back(0.0);
                
                
                
                Cerebellum *newFWD = new Cerebellum;
                
                string filenNameFWD("neuronDynamicsL1-50ms.dat");
                newFWD->setNeuronalDynamics(filenNameFWD);
                string connectionfilenNameFWD("networkConnectionsL1-50ms.dat");
                newFWD->setConnections(connectionfilenNameFWD);
                
                RMS *newRMS_FWDerror = new RMS;
                newRMS_FWDerror->setLength(1000);
                
                FWD.push_back(newFWD);
                RMS_FWDerror.push_back(newRMS_FWDerror);
                FWD_error.push_back(0.0);
        }

        return 1;
}

int Brain::runIteration()
{

        RefModel.setInput(0,Reference);
        RefModel.runIteration();


        Reference = Reference + CB_output;
        
        
        //Brainstem.setInput(0,Reference);
        //Brainstem.runIteration();
        //Motor_CMD = Brainstem.getOutput();
        Motor_CMD=DelayStem.feed(Reference);
        
        for (int i=0; i<FWD.size(); i++)
        {
                //TODO?? Match State
                FWD[i]->setInput(Motor_CMD);
                FWD[i]->updateNetwork();
        }
        
        CB_output = 0.0;
        
        for (int  j = 0; j < CB.size(); j++)
        {
                CB[j]->setInput(Motor_CMD);
                CB[j]->updateNetwork();
                
                CB_output += CB[j]->getSingleOutput()*Lambdas[j] ;
        }

        
}

int Brain::updateWeights()
{
        for (int  i = 0; i < FWD.size(); i++)
        {
                FWD[i]->updateOutputWeights(0.1,Lambdas[i],FWD_error[i]);
        }

        for (int  j = 0; j < CB.size(); j++)
        {
                CB[j]->updateOutputWeights(LEARNING_RATE,Lambdas[j],Error);
        }
        
}

void Brain::calculateErrors(double plantOutput)
{
        for (int  i = 0; i < FWD.size(); i++)
        {
                FWD_error[i] = -FWD[i]->getSingleOutput() + plantOutput;
                RMS_FWDerror[i]->feed(FWD_error[i]);
        }

        Error = RefModel.getOutput() - plantOutput;
}

double Brain::getOutput()
{
        return Motor_CMD;
}




//--------------Lambdas------------------//
/*int Brain::updateLambdas() //Hard Switch on smoothed FWD error
{
        double min = numeric_limits<double>::max();
        int min_index = 0;
        for (int i = 0; i < RMS_FWDerror.size(); i++)
        {
                Lambdas[i] = 0.0;
                if  ( fabs(RMS_FWDerror[i]->getAverage()) < min )
                {
                        min = fabs(FWD_error[i]);
                        min_index = i;
                }
        }
        Lambdas[min_index] = 1.0;
        
        if (min_index==0){Redcount+=1.0;}        
        else if (min_index==1){Greencount+=1.0;}        
        else if (min_index==2){Bluecount+=1.0;}
        
}*/
/*int Brain::updateLambdas()    //Hard Switch on unsmoothed FWD error
{
        double min = numeric_limits<double>::max();
        int min_index = 0;
        for (int i = 0; i < FWD_error.size(); i++)
        {
                Lambdas[i] = 0.0;
                if  ( fabs(FWD_error[i]) < min )
                {
                        min = fabs(FWD_error[i]);
                        min_index = i;
                }
        }
        Lambdas[min_index] = 1.0;
        
        if (min_index==0){Redcount+=1.0;}        
        else if (min_index==1){Greencount+=1.0;}        
        else if (min_index==2){Bluecount+=1.0;}
        
}*/
/*int Brain::updateLambdas()    //Softmax on unsmoothed Error
{
        double sigma = 0.001;
        double sum = 0.0;
        for (int i = 0; i < FWD_error.size(); i++)
        {
                Lambdas[i] =  (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(FWD_error[i],2.0)/(2.0*pow(sigma,2)));
                sum += Lambdas[i];
        }
        for (int j = 0; j < FWD_error.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }       
        Redcount += Lambdas[0];
        Greencount += Lambdas[1];
        Bluecount += Lambdas[2];
}*/
/*int Brain::updateLambdas()    //Softmax on smoothed Error
{
        double sigma = 0.001;
        double sum = 0.0;
        for (int i = 0; i < RMS_FWDerror.size(); i++)
        {
                Lambdas[i] =  (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(RMS_FWDerror[i]->getAverage(),2.0)/(2.0*pow(sigma,2)));
                sum += Lambdas[i];
        }
        for (int j = 0; j < RMS_FWDerror.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }       
        Redcount += Lambdas[0];
        Greencount += Lambdas[1];
        Bluecount += Lambdas[2];
}*/
/*int Brain::updateLambdas()      // Previous Lambda as prior
{
        double sigma = 1.0;
        double sum = 0.0;
        for (int i = 0; i < FWD_error.size(); i++)
        {
                if (Lambdas[i] == 0.0){Lambdas[i] =1.0;} 
                Lambdas[i] *= (1.0/(sqrt(2.0*M_PI)*sigma)*exp(-pow(FWD_error[i],2.0)/(2.0*pow(sigma,2)));
                sum += Lambdas[i];
        }
        for (int j = 0; j < FWD_error.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }       
        Redcount += Lambdas[0];
        Greencount += Lambdas[1];
        Bluecount += Lambdas[2];
}*/
int Brain::updateLambdas()      // Previous Lambda as prior
{
        double sigma = 0.005;
        double sum = 0.0;
        for (int i = 0; i < FWD_error.size(); i++)
        {
                if (Lambdas[i] == 0.0){Lambdas[i] =1.0;} 
                Lambdas[i] =pow(Lambdas[i],0.98) * (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(FWD_error[i],2.0)/(2.0*pow(sigma,2)));
                sum += Lambdas[i];
        }
        for (int j = 0; j < FWD_error.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }       
        Redcount += Lambdas[0];
        Greencount += Lambdas[1];
        Bluecount += Lambdas[2];
}
/*int Brain::updateLambdas()      // Previous Lambda as prior
{
        double sigma = 1.0;
        double sum = 0.0;
        for (int i = 0; i < FWD_error.size(); i++)
        {
                if (Lambdas[i] == 0.0){Lambdas[i] =1.0;} 
                Lambdas[i] =Lambdas[i]*0.8 + (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(FWD_error[i],2.0)/(2.0*pow(sigma,2)));
                sum += Lambdas[i];
        }
        for (int j = 0; j < FWD_error.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }       
        Redcount += Lambdas[0];
        Greencount += Lambdas[1];
        Bluecount += Lambdas[2];
}*/


int Brain::setLambdas(vector <double> lambdas)
{
        if (lambdas.size()==Lambdas.size())
        {
                Lambdas.clear();
                Lambdas = lambdas;
                return 1;
                }
                
        else if (lambdas.size() > Lambdas.size()) 
        {
                cout << "ERROR: Too Many Lambdas";
                return -1;
        }
        else    
        {
                cout << "ERROR: Too Few Lambdas" ;
                return -1;
        }
}



int Brain::getLambdas(vector<double> &lambdas)
{
        lambdas = Lambdas;
        return 1;
}



int Brain::printLambdas()
{
        cout <<  "\tLambdas (" << Lambdas.size() << ") :";
        for (int i=0; i<Lambdas.size(); i++)
        {
                cout << '\t' << Lambdas[i];
        }
        cout << endl;
}


//------------Reference-----------------//
int Brain::setRef(double reference)
{
        Reference = reference;        
}


double Brain::getRef()
{
        return Reference;        
}


int Brain::printRef()
{
        cout << Reference << '\t';
}

//-------------------------------------//

