if [ $# -eq 0 ] 
    then
        runFile=main.cpp
else
        runFile=$1
fi
        

if g++ -std=c++11 $runFile brain.cpp dynamics.cpp cerebellum.cpp neuron.cpp plant.cpp; then
        echo -e "\nCompiled..."
        ./a.out
else
        echo -e "\nCompile Failed...\n"
        exit
fi