#ifndef __DYNAMICS__H
#define __DYNAMICS__H

#include <iostream>
#include <vector>
#include <math.h>
using namespace std;


class DelayLine
{
public:
	DelayLine();
	void setLength(unsigned int length);
	void clear();
	double feed(double input);
	double getLength();
private:
	vector<double> theLine;
	unsigned int currentIndex;
};

class RMS
{
public:
        RMS();
        void setLength(unsigned int length);
        double feed(double in);
        double getAverage();
private:
        DelayLine Line;
        double average;
};


class Dynamics
{
public:
	/* Constructor of Neuron Dynamics
	 * 
	*/
	Dynamics();

	void setBk(vector <double> bk);
	void setAk(vector <double> ak);
	int  setBkAk(vector <double> bk, vector <double> ak);
	int  setIC(double IC);
	void printParameters();
	void printState();
        
        
        // Used when changing between Plants. Plant dynamics change but the State should continue
        int getState(vector<double> &ins,vector<double> &outs);
        int setState(vector<double> ins,vector<double> outs);

	/*
	 * resets the input and output history
	 * must be called before processing can start
	 * return 1 if configuration is ok
	 *	   -1 if size of bk and ak are of different length
	*/
	int  reset(void);


	/*
	 * returns 1 if processing of dynamics was successful
	 * returns -1 if processing failed due to configuration error
	 */
	int setInput(double in);

        
	double getOutput(void);
//private:
	vector <double> bks;
	vector <double> aks;
	vector <double> in;
	vector <double> out;
	void calcDynamics(void);

};


#endif 
