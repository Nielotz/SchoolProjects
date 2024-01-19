function [position, orientation, lin_speeds, rad_speeds] = ...
    direct_2DoF(pos, speed, arm_1_len, arm_2_len)
    
    adjusted_angular_pos = [pos(1);
                            pos(1) + pos(2)];
                            
    arms_lenghts = [arm_1_len;
                    arm_2_len];

    position = [sum(cos(adjusted_angular_pos) .* arms_lenghts); ...
                sum(sin(adjusted_angular_pos) .* arms_lenghts)];
    
    orientation = adjusted_angular_pos(end);

    adjusted_speed = [speed(1);
                      speed(1) + speed(2)];
    lin_speeds = [sum(-sin(adjusted_angular_pos) .* arms_lenghts); ...
                  sum(cos(adjusted_angular_pos) .* arms_lenghts) ...
                 ] .* adjusted_speed;
    
    rad_speeds = adjusted_speed(end);
end