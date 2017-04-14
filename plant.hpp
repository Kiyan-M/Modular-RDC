#ifndef __Plant__H
#define __Plant__H

#include <fstream>
#include <memory.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>

using namespace std;

class PlantRK
{
public:
        PlantRK();
        void runIteration(unsigned int nSteps);
        
        double rk4 ();
            double f_x (double x, double v, double Input);
            double f_v (double x, double v, double Input);

        //void choose(unsigned int subPlant_index);
        void setDynamics(double parameter);
        
        void setInput(double input);
        double getOutput();
        
private:
        double Input;
        vector <double> Input_history;
        vector <double> Input_weights;
        
        double x;
        double v;
        double a;
        unsigned int curr_Index;
        unsigned int new_Index; 
        
        
        double dt = 0.0001;
        // TODO: DESCRIBE Plant PARAMETERS!!
        
};

#endif

