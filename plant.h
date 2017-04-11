#ifndef __PLANT__H
#define __PLANT__H

#include "neuron.h"

#include <fstream>
#include <memory.h>
#include <string>
#include <vector>


class Plant
{
public:
        Plant();
        void setDynamics(string &filename);
        void choose(unsigned int subplant_index);
        void setInput(double input);
        void runIteration();
        double getOutput();
        void matchStates();
        
private:
        vector <Neuron *> subPlant;
        double Input;
        double Output;
        unsigned int curr_Index;
        unsigned int new_Index; 
        
};

#endif

