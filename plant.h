#ifndef __PLANT__H
#define __PLANT__H

#include "dynamics.h"
#include "neuron.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

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
        
        vector <Neuron *> subPlant;
        double Input;
        double Output;
        unsigned int curr_Index;
        unsigned int new_Index; 
        
private:
};

#endif

