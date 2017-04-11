#include "brain.h"

#define LEARNING_RATE 0.01

Brain::Brain(int numCerebellum)
{
        Lambdas.clear();
        C.clear();
        F.clear();

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
                
                //string filenName("neuronDynamics50-1ConstNum.dat");
                //newCerebellum->setNeuronalDynamics(filenName);
                //string connectionfilenName("networkConnections50-1ConstNum.dat");
                //newCerebellum->setConnections(connectionfilenName);
                newCerebellum->setNeuronalDynamics(1.0*M_PI,40.0*M_PI, 50);
                newCerebellum->setConnections();
	
                C.push_back(newCerebellum);
                Lambdas.push_back(0.0);
                
                
                
                Cerebellum *newF = new Cerebellum;
                
                //string filenNameF("neuronDynamics50-0p5ConstNum.dat");
                //newF->setNeuronalDynamics(filenNameF);
                //string connectionfilenNameF("networkConnections50-0p5ConstNum.dat");
                //newF->setConnections(connectionfilenNameF);
                newF->setNeuronalDynamics(20.0*M_PI,1000.0*M_PI, 5);
                newF->setConnections();
                
                DelayLine *newRMS_Ferror = new DelayLine;
                newRMS_Ferror->setLength(500);
                newRMS_Ferror->setDecayRate(exp(-0.000));
                
                F.push_back(newF);
                RMS_Ferror.push_back(newRMS_Ferror);
                F_error.push_back(0.0);
        }

        return 1;
}

int Brain::runIteration()
{

        RefModel.setInput(0,Reference);
        RefModel.runIteration();


        Reference = Reference + C_output;
        
        
        //Brainstem.setInput(0,Reference);
        //Brainstem.runIteration();
        //Motor_CMD = Brainstem.getOutput();
        Motor_CMD=DelayStem.feed(Reference);


        for (int i=0; i<F.size(); i++)
        {
                F[i]->setInput(Motor_CMD);
                F[i]->updateNetwork();
        }
        
        C_output = 0.0;
        for (int  j = 0; j < C.size(); j++)
        {
                C[j]->setInput(Motor_CMD);
                C[j]->updateNetwork();
                
                C_output += C[j]->getSingleOutput()*Lambdas[j] ;
        }

        
}

int Brain::updateWeights()
{
        for (int  i = 0; i < F.size(); i++)
        {
                F[i]->updateOutputWeights(0.1,Lambdas[i],F_error[i]);
        }

        for (int  j = 0; j < C.size(); j++)
        {
                C[j]->updateOutputWeights(LEARNING_RATE,Lambdas[j],Error);
        }
        
}

void Brain::calculateErrors(double plantOutput)
{
        for (int  i = 0; i < F.size(); i++)
        {
                F_error[i] = -F[i]->getSingleOutput() + plantOutput;
                RMS_Ferror[i]->feed(pow(F_error[i],2));
        }

        Error = RefModel.getOutput() - plantOutput;
}

double Brain::getOutput()
{
        return Motor_CMD;
}




//--------------Lambdas------------------//

/*int Brain::updateLambdas() //Modified Narendra
{
        Lambdas[0]=1.0;
}*/

/*int Brain::updateLambdas() //Exp Narendra
{
    double alpha = 0.5, Beta=0.5;
    double sum = 0.0, Cost;
    for (int i = 0; i < F_error.size(); i++)
    {
            Cost=alpha*F_error[i]+Beta*RMS_Ferror[i]->getDecayingAverage();
            if (Cost < 1e-12){Cost =1e-12;}
            Lambdas[i] = 1.0/Cost;
            if (Lambdas[i] < 1e-12){Lambdas[i] =1e-12;} 
            
            sum += Lambdas[i];
            
    }
    
    for (int j = 0; j < F_error.size(); j++)
    {
        Lambdas[j] = Lambdas[j]/sum;
    }
}*/

/*int Brain::updateLambdas() //Exp Narendra
{
    double alpha = 0.5, Beta=0.5;
    int mindex=0;
    double Cost, min=numeric_limits<double>::max();
    
    for (int i = 0; i < F_error.size(); i++)
    {
            Cost=alpha*pow(F_error[i],2);//+Beta*RMS_Ferror[i]->getDecayingAverage();
            if (Cost < min){mindex = i;}            
    }
    
    for (int j = 0; j < F_error.size(); j++)
    {
        if (j == mindex){
            Lambdas[j] = 1.0;
        }
        else
        {
            Lambdas[j] = 0.0;
        }
    }
}*/

int Brain::updateLambdas() //Modified Narendra
{
        double alpha = 0.5, Beta = 0.5;
        double sum = 0.0, Cost;
        for (int i = 0; i < F_error.size(); i++)
        {
                
                if (Lambdas[i] < 1e-12){Lambdas[i] =1e-12;} 
                Cost = alpha*pow(F_error[i],2) + Beta*RMS_Ferror[i]->getAverage()*500.0;
                if (Cost < 1e-12){Cost =1e-12;} 
                Lambdas[i] = 1.0/Cost;
                
                
                sum += Lambdas[i];
        }
        for (int j = 0; j < F_error.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }             
}
/*int Brain::updateLambdas() //Original Narendra
{
        double alpha = 0.5, Beta = 0.5;
        int mindex = 0;
        double minCost = 1e50, Cost;
        for (int i = 0; i < F_error.size(); i++)
        {
                Cost = alpha*pow(F_error[i],2) + Beta*RMS_Ferror[i]->getAverage()*500.0;
                if (Cost < minCost){
                        minCost = Cost;
                        mindex = i;
                }
                Lambdas[i] = 0.0;
        }
        Lambdas[mindex] = 1.0;
}*/

/*int Brain::updateLambdas()    //Hard Switch
{
        double min = numeric_limits<double>::max();
        int min_index = 0;
        for (int i = 0; i < F_error.size(); i++)
        {
                Lambdas[i] = 0.0;
                if  ( fabs(F_error[i]) < min )
                {
                        min = fabs(F_error[i]);
                        min_index = i;
                }
        }
        Lambdas[min_index] = 1.0;
        

}*/
/*int Brain::updateLambdas()    //Softmax (Wolpert)
{
        double sigma = 0.01;
        double sum = 0.0;
        for (int i = 0; i < F_error.size(); i++)
        {
                Lambdas[i] =  (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(F_error[i],2.0)/(2.0*pow(sigma,2)));
                if (Lambdas[i] < 1e-12){Lambdas[i] =1e-12;} 
                sum += Lambdas[i];
        }
        for (int j = 0; j < F_error.size(); j++)
        {
                Lambdas[j] = Lambdas[j]/sum;
        }       

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

