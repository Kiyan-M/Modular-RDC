#include "plant.h"

 PlantRK::PlantRK()
{

    Input_history.resize(3);
    
    Input_weights.push_back(+3.0/(0.002));
    Input_weights.push_back(-4.0/(0.002));
    Input_weights.push_back(+1.0/(0.002));
    
    x = 0.0;
    v = 0.0;
}

void PlantRK::runIteration(unsigned int nSteps)
{
    for (int i=0;i<nSteps;i++){
        rk4();
    }
}

double PlantRK::f_x (double x, double v, double Input)
{
    double d_input = 0.0;
    
    for (int i = 0; i < Input_history.size(); i++){
            d_input += Input_history[i]*Input_weights[i];
    }
    //cout << Input << '\t'<<Input_history[0]<< '\t'<<d_input<< endl;
    
    double dxdt = -a*x + d_input/1.463;
    
    return dxdt;

}

double PlantRK::f_v (double x, double v, double Input)
{
    //const double g = -9.81;
    //double l = 0.10;
    double m = 5.0;
    double c = 100.0;
    double k = 10000.0;
    
    //double dvdt = g*sin(x)/l + Input/(m*l*l);
    double dvdt = ( -c*v + -k*x + Input )/m;
    return 0.0;

}

double PlantRK::rk4 ()
{
  double f0_x;
  double f1_x;
  double f2_x;
  double f3_x;
  double f0_v;
  double f1_v;
  double f2_v;
  double f3_v;
  double t1;
  double t2;
  double t3;
  double dx1;
  double dx2;
  double dx3;
  double dv1;
  double dv2;
  double dv3;
//
//  Get four sample values of the derivative.
//
  f0_x = f_x (x, v, Input );
  f0_v = f_v (x, v, Input );

  dx1 = x + dt * f0_x / 2.0;
  dv1 = v + dt * f0_v / 2.0;
  f1_x = f_x (dx1, dv1, Input );
  f1_v = f_v (dx1, dv1, Input );

  dx2 = x + dt * f1_x / 2.0;
  dv2 = v + dt * f1_v / 2.0;
  f2_x = f_x (dx2, dv2, Input );
  f2_v = f_v (dx2, dv2, Input );

  dx3 = x + dt * f2_x / 2.0;
  dv3 = v + dt * f2_v / 2.0;
  f3_x = f_x (dx3, dv3, Input );
  f3_v = f_v (dx3, dv3, Input );
//
//  Combine to estimate the solution at time T0 + DT.
//
  x = x + dt * ( f0_x + 2.0 * f1_x + 2.0 * f2_x + f3_x ) / 6.0;
  v = v + dt * ( f0_v + 2.0 * f1_v + 2.0 * f2_v + f3_v ) / 6.0;
    
  return x;
}


void PlantRK::setDynamics(double parameter){a = parameter;}


void PlantRK::setInput(double input)
{
        Input = input;
        for (int i = 1; i < Input_history.size(); i++){
            Input_history[i] = Input_history[i-1];
        }
        Input_history[0] = input;
}


double PlantRK::getOutput(){return x;}


