% MIT 16.S498: Risk Aware and Robust Nonlinear Planning, Fall 2019
% Lecture 8: Nonlinear Robust Optimization
%% SOS Lyapunov Function Search for "uncertain" nonliner system dx(t)/dt=f(x,w) where w is a bounded uncertainty,  V(0)=0, V(x):SOS  -dV(x)/dt:SOS
% based on : Johan Lofberg "Modeling and solving uncertain optimization problems in YALMIP" 
% 17th World Congress, The International Federation of Automatic Control, Seoul, Korea, July 6-11, 2008

clc;clear all;close all

% 2d: Order of polynomial Lyapunov Function
d=2;

% variables
sdpvar x1 x2 w

% Uncertain nonlinear system dx(t)/dt=f(x,w)
f = [-1.5*x1^2-0.5*x1^3-x2;6*x1-w*x2];

% Polynomial Lyapunov Function or order 2d, c: coefficients, Vm: monomials
[V,c,Vm] = polynomial([x1;x2],2*d);

% dV(x)/dt
dVdt = jacobian(V,[x1;x2])*f;

%Uncertainty Set
F = [uncertain(w),3<=w<=5];

% SOS Conditions
r = [x1;x2]'*[x1;x2];
F = [F,sos(V-r),sos(-dVdt-r),c(1)==0];

%SDP solver
ops = sdpsettings('solver','mosek');

% Solve SOS based SDP
solvesdp(F,[],ops,c);


%% Results
% Obtained coefficients of polynomial Lyapunov Function
cc=value(c);

% Lyapunov Function
L1=sdisplay(cc'*Vm);
% dV(x)/dt
dL1 = sdisplay(jacobian(cc'*Vm,[x1;x2])*f);

%% Plots

L2=strrep(strrep(L1,'*','.*'),'^','.^');L3=cell2mat((L2));
dL2=strrep(strrep(dL1,'*','.*'),'^','.^');dL3=cell2mat((dL2));

[x1,x2]=meshgrid([-1:0.01:1],[-1:0.01:1]);
% Lyapunov Function
surf(x1,x2,eval(L3),'FaceColor','blue','FaceAlpha',0.85,'EdgeColor','none','FaceLighting','phong');hold on;grid on;

%dV(x)/dt  for w=3;
w=3;
surf(x1,x2,eval(dL3),'FaceColor','red','FaceAlpha',0.85,'EdgeColor','none','FaceLighting','phong');hold on;grid on;
surf(x1,x2,zeros(size(eval(L3))),'FaceColor','black','FaceAlpha',0.85,'EdgeColor','none','FaceLighting','phong');hold on;grid on;

camlight; lighting gouraud; axis square
ax = gca; ax.FontSize = 30;
xlabel('$x_1$','Interpreter','latex', 'FontSize',40);ylabel('$x_2$','Interpreter','latex', 'FontSize',40);
str1 = '$V(x)$';text(0.5,0.5,40,str1,'Interpreter','latex','FontSize',30)
str1 = '$\frac{d V(x,\omega)}{d t}$';text(0.5,0.5,-200,str1,'Interpreter','latex','FontSize',30) 

