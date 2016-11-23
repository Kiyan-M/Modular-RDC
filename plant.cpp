#include "plant.h"

 Plant::Plant()
{

}


void Plant::setDynamics(string &fileName)
{

        string line; //
        ifstream myfile (fileName.c_str());
        if (myfile.is_open())
        {
                bool isBk=true; //the first line is a bk parameter
	        vector <double> aks;
	        vector <double> bks;
	        aks.clear();
	        bks.clear();
	        
                while (myfile.good())
                {       
                        getline(myfile,line); // Copies next line from "myfile" into "line" (looks for '\n' delimiter)
                        char *a=new char[line.size()+1];
                        a[line.size()]=0;
                        memcpy(a, line.c_str(), line.size()); // Copies "line" into char array "a"
                        
                        char *dummy;
                        int k=0;
                        float value;
                        dummy=strtok(a,"\t "); // Copies "a" until delimiter '\t' or ' ' into dummy
                        
                        
                        while(dummy!=NULL) // While dummy is not NULL character put at the end of each line
                        {
                                sscanf(dummy,"%f", &value);// converts char in "dummy" to float in "value"
                                
                                if (isBk)  {bks.push_back(value);} // Add to bk
                                else       {aks.push_back(value);} // Add to ak
                                
                                dummy=strtok(NULL,"\t "); // converts char in "dummy" to float in "value"
                        }
                        isBk=!isBk; // at the end of line (dummy=NULL) bk changes to ak, or vice versa
                        
                        if (isBk) // If ak has just turned to bk we have a complete pair ok aks andd bks
                        {
                                Neuron *singleNeuron = new Neuron;
                                
                                singleNeuron->setDynamics(bks,aks);                       
                                singleNeuron->setSize(1);
                                singleNeuron->setWeight(0,1.0);

                                subPlant.push_back(singleNeuron);


                                //clear bks and aks
                                aks.clear();
                                bks.clear();
                        }
                        
                }
        myfile.close();
        }
        
        curr_Index = 0; // Select first subPlant

}


void Plant::runIteration()
{
        subPlant[curr_Index]->setInput(0,Input);        
        subPlant[curr_Index]->runIteration();
        
        Output = subPlant[curr_Index]->getOutput();
}


void Plant::choose(unsigned int new_index)
{
        if (new_index < subPlant.size())
        {
                
                new_Index = new_index;
                matchStates();
                curr_Index = new_Index;
                cout << "Choosing Plant " << new_Index << endl;
                
        }
        else
        {
                cout << "Index out of range" << endl;
        }
}

void Plant::matchStates()
{
        vector<double> in;
        vector<double> out;
        
        subPlant[curr_Index]->getState(in,out);
        
        subPlant[new_Index]->setState(in,out);
}

void Plant::setInput(double input)
{
        Input = input;
}


double Plant::getOutput()
{
        return Output;
}
