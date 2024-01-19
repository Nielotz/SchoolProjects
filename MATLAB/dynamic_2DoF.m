%
% return format:
%   arm1 [at1, at2, at3, ...]
%   arm2 [at1, at2, at3, ...]
function [R_arm_orientation, R_arm_speed_rad] = dynamic_2DoF(mass, arm_len, moments, ...
    pos_0, speed_0, moments_dt)
iterations = size(moments, 2);

R_arm_orientation = zeros(2, iterations);
R_arm_speed_rad = zeros(2, iterations);

%% Solve second order dynamic movement differential equation.
% M * q.. + C * q. = u = moment
%
% Substitute q. with z:
%     z = q. = speed
%     z. = M^-1 * (u - C*z) = acceleration
% dzdt = @(u, z, M, C) M \ (u - C*z);

R_arm_orientation(:, 1) = pos_0;
R_arm_speed_rad(:, 1) = speed_0;

h = moments_dt;
for i = 1 : iterations - 1
    moment = moments(:, i);
    speed = R_arm_speed_rad(:, i);
    pos = R_arm_orientation(:, i);

    % s1 = h * F(t,y);
    [M, C] = calculate_M_C(mass, arm_len, pos, speed);
    [pos1, speed1] = calculate_step(h, M, C, moment, speed);

    % s2 = h * F(t+h/2, y+s1/2);
    [M, C] = calculate_M_C(mass, arm_len, pos + pos1/2, speed + speed1/2);
    [pos2, speed2] = calculate_step(h, M, C, moment, speed + speed1/2);

    % s3 = h * F(t+h/2, y+s2/2);
    [M, C] = calculate_M_C(mass, arm_len, pos + pos2/2, speed + speed2/2);
    [pos3, speed3] = calculate_step(h, M, C, moment, speed + speed2/2);

    % s4 = h * F(t+h, y+s3);
    [M, C] = calculate_M_C(mass, arm_len, pos + pos3, speed + speed3);
    [pos4, speed4] = calculate_step(h, M, C, moment, speed + speed3);


    R_arm_orientation(:, i+1) = pos ...
        + (1 / 6) * (pos1 + 2*pos2 + 2*pos3 + pos4);
    R_arm_speed_rad(:, i+1) = speed ...
        + (1 / 6) * (speed1 + 2*speed2 + 2*speed3 + speed4);
end
end

% Return format:
%   X(1) = [t1, t2, t3...]
%   X(2) = [t1, t2, t3...]
function [R_arm_orientation, R_arm_speed_rad] = calculate_step(...
    h, ...
    M, C, ...
    moment, ...
    arm_speed_rad ...  % [1; 2] Angular position of the joints [rad/sec].
    )
    R_arm_orientation = h * arm_speed_rad;
    R_arm_speed_rad = h * (M \ (moment - C*arm_speed_rad));
end