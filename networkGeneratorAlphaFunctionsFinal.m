clear all
close all




%%generator output file names
dynamicsFilename='neuronDynamicsCere1.dat'; %%specifies the dynamics of the alpha functions
connectionsFilename='networkConnectionsCere1.dat'; %%specifies the connectivity



%%seed the random number generator
rand('seed',55);

numOfNeurons=25;
numberOfNeuronOutputConnections=0;

%%specify here (0 or 1) if you want or don't want negative feedback
%%connections'v'
addFeedback=0;
feedbackWeight=0.0;



%%system sample time
T=0.001; 
Fmax=(1/T) /2;

%time constant range of the alpha functions  in seconds
maxTimeConstant=100E-3;
minTimeConstant=10E-3;

%%create a linearly spaced alpha function array
timeConstantRange=maxTimeConstant-minTimeConstant;


k=minTimeConstant+timeConstantRange*rand(numOfNeurons,1);
%;
connectionData=[-1]; %%specifies the cerebellar input 
feedbackConnectionData=[-3] %%specifies the cerebellar output

%% for documentation reasons we run the step response of all filters
%% we prepare this here and then plot in the loop below 
stepNumber=500; %%number of filter iterations
impulseInput=zeros(1,stepNumber);
impulseInput(1)=1; 
impulseTime=1:stepNumber;
figure(1) %%is the figure that will display the filter step responses
xlabel('time in ms','FontSize',16);
ylabel('Impulse response', 'FontSize',16)
set(gca,'fontsize',16) %font size of axis numbers
hold on;
for i=1:numOfNeurons
    
    %%[bk,ak]=butter(2,k(i)/Fmax);
    
    %%alpha function 
    %%Gk(s) =
    %       1
    %-------------
    %   (Tks + 1)^2
    tau=k(i);
    
    den=[tau^2  2*tau  1]
    num=[ 1];
    sys=tf(num,den)
    sysd=c2d(sys,T);
    
    
    [bk,ak] =tfdata(sysd,'v')
    
    
    if (i==1)
       dynamicsData=[bk; ak]
       
    else
     dynamicsData=[dynamicsData ; bk ; ak;]
    end
    
    

    [y] = filter(bk,ak,impulseInput)
    
    plot(impulseTime,y,'Color','black','LineWidth',2);
    
    
   
    
    
    
        
    %%for now non-connected network, each neuron receives only the 
    %%cerebellar input, we add this to the other neuronal inputs from above
     connectionData=[ connectionData  i-1 1]
     
     %%the same is done for the negative feedback connections
     feedbackConnectionData=[feedbackConnectionData i-1 feedbackWeight]
     
    
end

hold off;


allDynamicCutOffFrequencies=k';

%%now we go through all neurons and generate 
%%a list of output targets for each

numberOfNeuronOutputConnections


for i=1:numOfNeurons
    outputConnectionList=-2*ones(1,numOfNeurons)
    outputWeightList=zeros(1,numOfNeurons)
    targetList=-2*ones(1,numOfNeurons*2)
    for k=1:numberOfNeuronOutputConnections
        
        targetNeuronIndex=int16(rand(1)*(numOfNeurons-1));
        
        %%note, with the weight we can make sure we only have negative
        %%feedback
        %% neuronalWeight=-1*(rand(1)-0.5); %%mixed weights (pos and neg)
         neuronalWeight=-0.8*(rand(1)); % negative weights only
        outputConnectionList(k)=targetNeuronIndex;
        outputWeightList(k)=neuronalWeight;
        targetList((k*2)-1)=targetNeuronIndex;
        targetList((k*2))=neuronalWeight;
        
    end
    targetList=[i-1 targetList];
    connectionData=[connectionData; targetList]
    
end
if (addFeedback==1)
    connectionData=[connectionData; feedbackConnectionData]
end
plotNeuronalInterconnect(connectionData,999)

%%for testing of the network we add a second line which specifies the 
%% the inputs neuron 3 is connected to
%%connectionData=[connectionData; connectionData]
%%connectionData(2,1)=3
%%dynamicsData=[1 2; 3 4; 5 6; 7 8]
dlmwrite(dynamicsFilename,dynamicsData,' ')

dlmwrite(connectionsFilename,connectionData,' ')

%%output ref model data

