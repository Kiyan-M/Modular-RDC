#include "brain.h"

#define LEARNING_RATE 0.01

Brain::Brain()
{
        Lambdas.clear();
        CB.clear();

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
        
       
        
}

int Brain::addCerebellum()
{
        Cerebellum *newCerebellum = new Cerebellum;
        
        //string filenName("neuronDynamicsCere1.dat");
        string filenName("neuronDynamicsAlex10-100ms.dat");
        newCerebellum->setNeuronalDynamics(filenName);
        
        //string connectionfilenName("networkConnectionsCere1.dat");
        string connectionfilenName("networkConnectionsAlex10-100ms.dat");
        newCerebellum->setConnections(connectionfilenName);
	
        CB.push_back(newCerebellum);
        Lambdas.push_back(0.0);
        
        
        return 1;
}

int Brain::runIteration()
{

        Reference = Reference + CB_output;
        
        //Brainstem.setInput(0,Reference);
        //Brainstem.runIteration();
        
        //Motor_CMD = Brainstem.getOutput();
        
        
        Motor_CMD=DelayStem.feed(Reference);        
        
        CB_output = 0.0;
        for (int  i = 0; i < CB.size(); i++)
        {
                CB[i]->setInput(Motor_CMD);
                CB[i]->updateNetwork();
                
                CB_output += CB[i]->getSingleOutput()*Lambdas[i] ;
        }

        
}

int Brain::updateWeights()
{
        for (int  i = 0; i < CB.size(); i++)
        {
                CB[i]->updateOutputWeights(LEARNING_RATE,Lambdas[i],Error);
                
        }
        
}


double Brain::getOutput()
{
        return Motor_CMD;
}

//--------------Lambdas------------------//
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

