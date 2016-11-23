#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "dynamics.h"
#include "neuron.h"
#include "plant.h"

#define LOG_DATA 1

int main(int argc, char *argv[])
{
        cout << endl << endl;
        Plant plant;
        string fileName("testDynamics.dat");
        plant.setDynamics(fileName);
        
        double output;
        
#if LOG_DATA == 1
        ofstream temp_log;
        temp_log.open("logTemp.dat");
        
        
        temp_log << "time" << '\t' 
                 << "Reference" << '\t' 
                 << "Response" << endl;
#endif

        for (int i = 0; i<1000; i++){
        
                plant.setInput(1.0);
                plant.runIteration();
                output = plant.getOutput();
                if (i==100)     {plant.choose(1);}
                else if (i==200) {plant.choose(0);}
                
#if LOG_DATA == 1
        temp_log << i << '\t' 
                 << 1.0 << '\t' 
                 << output << endl;
#endif

        }
        cout << endl << endl;
        
}
