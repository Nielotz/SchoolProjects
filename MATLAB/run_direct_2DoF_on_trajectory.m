function [time, position, orientation, lin_speed, rad_speed] = ...
    run_direct_2DoF_on_trajectory(time, pos, speed, arm)

    arm1 = arm(1);
    arm2 = arm(2);

    iterations = size(time, 1);

    position = zeros(iterations, 2);
    orientation = zeros(iterations, 1);
    lin_speed = zeros(iterations, 2);
    rad_speed = zeros(iterations, 1);

    for i = 1:iterations
        [po, orien, lin_s, rad_s] = ...
            direct_2DoF(pos(i, :)', speed(i, :), arm1, arm2);
        
        position(i, :) = po;
        orientation(i) = orien;
        lin_speed(i, :) = lin_s;
        rad_speed(i) = rad_s;
    end
end