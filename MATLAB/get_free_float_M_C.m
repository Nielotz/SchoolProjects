%--------------------------------------------------------------------------
% Function for calculating functioncs calculating M and C matrices
%--------------------------------------------------------------------------

function [M, C] = get_free_float_M_C( ...
    p1, ...  % manipulator mounting point in x axis
    p2, ...  % manipulator mounting point in y axis
    m0, ...  % the mass of the satellite
    m1, ...  % the mass of the first kinematic pair
    m2, ...  % the mass of the second kinematic pair
    a1, ...  % center of mass of the first link
    a2, ...  % center of mass of the second link
    L1, ...  % the length of the first link
    L2, ...  % the length of the second link
    I0, ...  % Satellite moment of inertia
    I1, ...  % Moment of inertia of the first kinematic pair
    I2)      % Moment of inertia of the second kinematic pair

syms x1  % x-positon of the satellite's center of mass [m]
syms x2  % y-positon of the satellite's center of mass [m]
syms x3  % satellite orientation [rad]
syms x4  % angular position of the first joint [rad]
syms x5  % angular position of the second joint [rad]
syms x6  % the first derivative of x1 [m/sec]
syms x7  % the first derivative of x2 [m/sec]
syms x8  % the first derivative of x3 [rad/sec]
syms x9  % the first derivative of x4 [rad/sec]
syms x10  % the first derivative of x5 [rad/sec]


% Base (Satellite)
J0v = [1 0 0 0 0 ;
    0 1 0 0 0 ];
J0vt = J0v';
J0w = [0 0 1 0 0 ];
J0wt = J0w';

% First link
var1 = - p1*sin(x3) -p2*cos(x3) - a1*sin(x3+x4);
var2 = -a1*sin(x3+x4);
var3 = -p2*sin(x3) + p1*cos(x3)+a1*cos(x4+x3);
var4 = a1*cos(x4+x3);
J1v = [1 0  var1  var2 0 ;
    0 1  var3  var4 0 ];
J1vt = J1v';
J1w = [0 0 1 1 0];
J1wt = J1w';

% Second link
var5 = -p2*cos(x3) - L1*sin(x4+x3) - p1*sin(x3) - a2*sin(x4+x5+x3);
var6 = -L1*sin(x4+x3) - a2*sin(x4+x5+x3);
var7 = -a2*sin(x4+x5+x3);
var8 = -p2*sin(x3) + a2*cos(x4+x5+x3) + L1*cos(x4+x3)+p1*cos(x3);
var9 =  a2*cos(x4+x5+x3) + L1*cos(x4+x3);
var10 = a2*cos(x4+x5+x3);
J2v = [1 0   var5 var6  var7 ;
    0 1   var8 var9 var10 ];
J2vt = J2v';
J2w = [0 0 1 1 1 ];
J2wt = J2w';


% M matrix
Tv = m0*J0vt*J0v +  m1*J1vt*J1v +  m2*J2vt*J2v;
Tw = J0wt*I0*J0w +  J1wt*I1*J1w +  J2wt*I2*J2w;
M  = Tv+Tw;


% C matrix
Qdot = [x6; x7; x8; x9; x10];
Qdott =[x6 x7 x8 x9 x10 ];
Q = [x1; x2; x3; x4; x5];

N1 = sym(zeros(size(Qdot,1), size(Qdot,1)));
for i=1:1:size(Qdot,1)
    for j=1:1:size(Qdot,1)
        sum = 0;
        for k=1:1:size(Qdot,1)
            sum = sum + diff(M(i,j),Q(k))*Qdot(k);
        end
        N1(i,j)=sum;
    end
end

N2 = sym(zeros(size(Qdot,1), 1));
for i=1:1:size(Qdot,1)
    P = sym(zeros(size(Qdot,1), size(Qdot,1)));
    for j=1:1:size(Qdot,1)
        for k=1:1:size(Qdot,1)
            P(j,k) = diff(M(j,k),Q(i));
        end
    end
    N2(i,1) = Qdott*P*Qdot;
end

C = N1*Qdot - 0.5*N2;

end
