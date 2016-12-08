#include "dynamics.h"


DelayLine::DelayLine()
	{
		theLine.clear();
		currentIndex=0;
	}

	void DelayLine::setLength(unsigned int length)
	{
		theLine.clear();
		theLine.resize(length);
		currentIndex=0;

	}
	void DelayLine::clear()
	{
		for (int i=0;i<theLine.size();i++)
		{
			theLine[i]=0.0;
		}
		currentIndex=0;
	}


	double DelayLine::feed(double input)
	{
		double retValue;
		retValue=theLine[currentIndex]; //return oldest value
		theLine[currentIndex]=input; //put newest value into current index
		if (currentIndex<theLine.size()-1)
		{
			currentIndex++;
		}
		else
		{
			currentIndex=0;
		}
		return retValue; //the oldest value in the pipe

	}
	
	double DelayLine::getLength()
	{
	        return (double)theLine.size();
	}

RMS::RMS()
{
        Line.clear();
        average = 0.0;
}
        void RMS::setLength(unsigned int length)
        {
                Line.setLength(length);
        }
        
        double RMS::feed(double in)
        {
                in = pow(in,2);
                double out;
                out = Line.feed(in);
                average += (in-out)/((double)Line.getLength());
                
                return average;
        }
        double RMS::getAverage()
        {
                return average;
        }


Dynamics::Dynamics()
{
		bks.clear();
		aks.clear();
}

int Dynamics::reset(void)
{
        
	int retvalue=1;
	if (bks.size() == aks.size())
	{
		in.resize(bks.size());
		out.resize(bks.size());
		for (unsigned int i=0;i<bks.size();i++)
		{
			in[i]=0.0; 
			out[i]=0.0;
		}
	}
	else
	{
		retvalue=-1;
	}

	return retvalue;
}


int Dynamics::setInput(double input)
{
	//lets shift the data first
	//with the old data falling out
	for (unsigned int i=in.size()-1;i>0;i--)
	{
		in[i]=in[i-1];
		out[i]=out[i-1];
	}
		out[0]=0;
		in[0]=input;

	//now perform the filter function

	for (unsigned int i=0; i<in.size();i++)
	{
		out[0]= out[0]+ bks[i]*in[i]-aks[i]*out[i];
	}
	return 1;	
}

double Dynamics::getOutput(void)
{
	return out[0];
}
void Dynamics::setBk(vector <double> bk)
{
	bks.clear();

	bks=bk;
}

void Dynamics::setAk(vector <double> ak)
{
	aks.clear();

	aks=ak;
}

int Dynamics::setBkAk(vector <double> bk, vector <double> ak)
{
	if (bk.size()==ak.size())
	{
		setBk(bk);
		setAk(ak);
		reset();
		return 1;
	}
	else
	{
		return -1;
	}
}


int Dynamics::setIC(double IC)
{
        if (aks.size() > 0){                
	        for (int i = 0; i < aks.size(); i++){
	                in[i]  = IC;
	                out[i] = IC;
	        }
        }
        else
        {
                cout << "Error! size of as and bs not yet known" << endl;
                return -1;
        }
        
}


void Dynamics::printParameters()
{
	cout<<" #bk: "<<bks.size() <<"   #ak: "<<aks.size()<<endl;
	if (bks.size() == aks.size())
	{
		for (unsigned int i=0; i<bks.size(); i++)
		{
			cout<<"b"<<i<<"=  "<<bks[i]<<"     a"<<i<<"=  "<<aks[i]<<endl;
			//cout<<"b"<<i<<endl;
		}
	}
	else
	{
		cout<<" Number of parameters are not equal!"<<endl;
	}
}

void Dynamics::printState()
{
        cout << " #in: " << in.size() << "\t#out " << out.size() << endl;
	if (in.size() == out.size())
	{
		for (unsigned int i=0; i<in.size(); i++)
		{
			cout<<"in"<<i<<"=  "<<in[i]<<"     out"<<i<<"=  "<<out[i]<<endl;
			//cout<<"b"<<i<<endl;
		}
	}
	else
	{
		cout<<" Number of ins and outs are not equal!"<<endl;
	}
        
        
}


int Dynamics::getState(vector<double> &ins,vector<double> &outs)
{
        if (in.size() == out.size())
	{
		ins = in;
		outs = out;
		return 1;
	}
	else
	{
		cout<<" Number of ins and outs are not equal!"<<endl;
		return -1;
	}
}


int Dynamics::setState(vector<double> ins,vector<double> outs)
{
        if (ins.size() == outs.size())
	{
		in = ins;
		out = outs;
		return 1;
	}
	else
	{
		cout<<" Number of ins and outs are not equal!"<<endl;
		return -1;
	}
}
