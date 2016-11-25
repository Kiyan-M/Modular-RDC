#include "dynamics.h"

int main(int argc, char *argv[])
{
        cout << "hello world" << endl;
        Smooth A;
        A.setSize(3);
        
        A.setInput(5.0);
        A.getOutput();
        A.setInput(0.0);
        A.getOutput();
        A.setInput(1.0);
        A.getOutput();
        A.setInput(2.0);
        A.getOutput();
        A.setInput(10.0);
        A.getOutput();
        A.setInput(2.0);
        A.getOutput();

}
