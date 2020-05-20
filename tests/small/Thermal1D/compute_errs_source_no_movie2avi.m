close all; 
clear all; 

%define parameters 
kappa = 1.6; %thermal conductivity
c = 1.0; %heat capacity 
rho = 1.0; %density 
a = 16.0;
dt = 0.01; %time step
tf = 1.0; %final time 
t = [0:dt:tf]; 
dx = 1/20; 
x = [0:dx:1]; 
T = ncread('thermal1D_with_source_out.exo', 'vals_nod_var1'); 
%size(T)
Texact = 0*T; 
for i=1:length(x)
  Texact(i,:) = a*x(i)*(1.0-x(i))*cos(2.0*pi*kappa*t/rho/c); 
end  
%size(Texact) 

K = length(t); 
fig1=figure(1);
winsize = get(fig1,'Position');
winsize(1:2) = [0 0];
Movie=moviein(K,fig1,winsize);
set(fig1,'NextPlot','replacechildren')

rel_err = []; 
 
for i=1:K
  set(gca,'NextPlot','replacechildren') ; 
  plot(x, T(:,i), 'b'); 
  hold on; 
  plot(x, Texact(:,i), '--r'); 
  err = norm(T(:,i)-Texact(:,i))/norm(Texact(:,i)); 
  rel_err = [rel_err; err]; 
  xlabel('x'); 
  ylabel('Temp'); 
  legend('Computed','Exact', 'Location','NorthWest'); 
  title(['Temperature solution at t = ', num2str(t(i))]); 
  axis([0 1 0 4])  
  pause(0.1)
  %Movie(:,i)=getframe(fig1,winsize);
  %mov(i) = getframe(gcf);
  %n = strcat("thermal_", num2str(i)); 
  %name = strcat(n, ".png") 
  %saveas(gcf,name)
end 

%To convert the pngs to mp4, run:
%  ffmpeg -framerate 10 -start_number 1 -i thermal_%1d.png -r 5 -vf scale=-2:1080,setsar=1 -pix_fmt yuv420p thermal_MMS.mp4

X = ['Average relative error over time = ', num2str(mean(rel_err))]; 
disp(X) 
disp([' ']); 
figure(); 
plot(t, rel_err); 
xlabel('time'); 
ylabel('Relative Error'); 
title('Relative Errors w.r.t. Exact Solution');  


