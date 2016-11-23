%%simple Cerebellar Neuron Printer

close all
cerebellumData=importdata('testOutput.txt');
weightData=importdata('testWeight.txt');

numberOfExtraData=5

figure(100);
set(gca,'fontsize',16) %font size of axis numbers
plot(weightData);
xlabel('time in ms');
title('Cerebellar output weights')
time=1:length(cerebellumData);

NeuronalState=cerebellumData(:,numberOfExtraData:end);

figure(11)
plot(NeuronalState);
title('Neuronal State')






figure(2)
plot(cerebellumData(:,1)); title('Filtered Random Input')
xlabel('time in ms');
figure(3)
plot(cerebellumData(:,2)); title('Plant Input')
xlabel('time in ms');
figure(33)
plot(time, cerebellumData(:,2), time, cerebellumData(:,1)); title('Plant Input and random system input')
xlabel('time in ms');


figure(4)
plot(cerebellumData(:,3)); title('Plant Output')
xlabel('time in ms');
figure(5)
plot(cerebellumData(:,4)); title('Sensory Error')
xlabel('time in ms');


figure(66);
plot(time,cerebellumData(:,1), time, cerebellumData(:,3))
title('reference input and plant output')
xlabel('time in ms');


normalisedReference=cerebellumData(:,1)
maxRef=max(normalisedReference);
normalisedReference=normalisedReference/maxRef;


normalisedPlant=cerebellumData(:,3)
normalisedPlant=normalisedPlant/maxRef;


figure(1000) %%is the figure that will display the filter step responses

set(gca,'fontsize',16) %font size of axis numbers
plot(time,normalisedReference,'Color',[0.8 0.8 0.8],'LineWidth',4)
hold on
legend('normalised reference signal')
plot( time,normalisedPlant,'Color','black','LineWidth',1)
legend('normalised plant output')
hold off
legend('normalised plant output')
xlabel('time in ms','FontSize',16);
ylabel('Normalised output', 'FontSize',16)


%%plot of first comparePercentage% and last comparePercentage%

%define comparison section, i.e. compare first 10% with last 10% of
%experiment, comparePercentage=10 %i.e. 10%

comparePercentage=1;
comparlength=comparePercentage/100;
plotlength=floor( comparlength * length( normalisedReference))

earlystart=1
earlyend=plotlength

latestart=length(normalisedReference)-plotlength
lateend=length(normalisedReference)

figure(1001);

subplot(2,1,1)
set(gca,'fontsize',16) %font size of axis numbers
plot(time(earlystart:earlyend),normalisedReference(earlystart:earlyend),'Color',[0.8 0.8 0.8],'LineWidth',4)
hold on
plot( time(earlystart:earlyend),normalisedPlant(earlystart:earlyend),'Color','black','LineWidth',1)
hold off
legend( 'normalised reference','normalised plant output')
xlabel('time in ms','FontSize',16);
ylabel('Normalised output', 'FontSize',16)
titletext=sprintf('First %d %% of Experiment', comparePercentage)
title(titletext)


subplot(2,1,2)
set(gca,'fontsize',16) %font size of axis numbers
plot(time(latestart:lateend),normalisedReference(latestart:lateend),'Color',[0.8 0.8 0.8],'LineWidth',4)
hold on
plot( time(latestart:lateend),normalisedPlant(latestart:lateend),'Color','black','LineWidth',1)
hold off
legend( 'normalised reference','normalised plant output')
xlabel('time in ms','FontSize',16);
ylabel('Normalised output', 'FontSize',16)
titletext=sprintf('Last %d %% of Experiment', comparePercentage)
title(titletext)
hold off










%%plot the rms error
% 
% 
% 
% 
 RMSlength=1000;
 errorData=cerebellumData(:,4);
 for i=1:length(errorData)-(RMSlength+1)
     RMSerror(i)=norm(errorData(i:i+RMSlength));
 end
 
 RMSerror=RMSerror/max(RMSerror)
 %
 
figure(1002)
set(gca,'fontsize',16) %font size of axis numbers
 plot(RMSerror,'Color','black','LineWidth',2);
 xlabel('time in ms');
title('Normalised RMS of sensory error')
% 
% 
% 
% %%now lets plot the weights as a function of the neuronal cut-off frequency
% 
% 
% %%figure(4)
% %%
% [rows,cols]=size(weightData)
% finalWeights=weightData(rows,:)
% figure(123);stem(allDynamicCutOffFrequencies,finalWeights);title('weight of filters')
% xlabel('cut-off frequency'); ylabel('weight');