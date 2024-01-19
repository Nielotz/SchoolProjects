function [M, C] = calculate_M_C(mass, ...
                                arm_len, ...
                                pos, ...
                                speed)
    mass2_arm1_arm2 = mass(2) * arm_len(1) * arm_len(2); 
    
    m2_l1_l2_cos_pos2 = mass2_arm1_arm2 * cos(pos(2));
    arm_2_moment_of_inertia = (1 / 3) * mass(2) * arm_len(2) * arm_len(2); 

    Mass11 = (mass(1)/3 + mass(2)) * arm_len(1) * arm_len(1) ...
             + arm_2_moment_of_inertia + m2_l1_l2_cos_pos2;
    Mass12 = arm_2_moment_of_inertia + (1 / 2) * m2_l1_l2_cos_pos2;
    Mass21 = Mass12;
    Mass22 = arm_2_moment_of_inertia;

    Coriolise11 = 0;
    Coriolise12 = -mass2_arm1_arm2 * (speed(1) + speed(2)/2) * sin(pos(2));
    Coriolise21 =  (1 / 2) * mass2_arm1_arm2 * speed(1) * sin(pos(2));
    Coriolise22 = 0;

    M = [Mass11 Mass12; ...
         Mass21 Mass22];

    C = [Coriolise11 Coriolise12; ...
         Coriolise21 Coriolise22];
end