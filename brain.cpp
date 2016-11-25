#include "brain.h"

#define LEARNING_RATE 0.01

Brain::Brain(int numCerebellum)
{
        // TODO cout << numCerebellum << endl << endl;
        Lambdas.clear();
        CB.clear();
        FWD.clear();

        // Create First FF Controller
        addCerebellum();

        
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

int Brain::addCerebellum()
{


        Cerebellum *newCerebellum = new Cerebellum;
        
        string filenName("neuronDynamicsAlex10-100ms.dat");
        newCerebellum->setNeuronalDynamics(filenName);
        string connectionfilenName("networkConnectionsAlex10-100ms.dat");
        newCerebellum->setConnections(connectionfilenName);
	
        CB.push_back(newCerebellum);
        Lambdas.push_back(0.0);
        
        
        
        Cerebellum *newFWD = new Cerebellum;
        
        string filenNameFWD("neuronDynamicsAlex10-100ms.dat");
        newFWD->setNeuronalDynamics(filenNameFWD);
        string connectionfilenNameFWD("networkConnectionsAlex10-100ms.dat");
        newFWD->setConnections(connectionfilenNameFWD);
        
        FWD.push_back(newFWD);
        FWD_error.push_back(0.0);
        
        
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
        }

        Error = RefModel.getOutput() - plantOutput;
}

double Brain::getOutput()
{
        return Motor_CMD;
}




//--------------Lambdas------------------//
int Brain::updateLambdas()
{
//TODO implement infinity
        double min = 100.0;
        int min_index = 0;
        for (int i = 0; i < FWD.size(); i++)
        {
                Lambdas[i] = 0.0;
                if  ( fabs(FWD_error[i]) < min )
                {
                        min = fabs(FWD_error[i]);
                        min_index = i;
                }
        }
        Lambdas[min_index] = 1.0;
        
}

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

