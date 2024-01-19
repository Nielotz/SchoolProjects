    clear all;

%% Simulation properties.
arm_length = [
    0.6;
    0.5];
arm_mass = [
    2.5;
    1.5];
arm_pos_0 = [
    0; 
    0];
arm_speed_0 = [
    0; 
    0];
satellite_mass = 35 * ((2.5 + 1.5) / 35) * 10;
satellite_inertia = 1.5 * ((2.5 + 1.5) / 35);

TOTAL_TIME = 15;
DT = 0.001;

%% Trajectory generatrion.
[time1, pos1, speed1, acceleration1] = my_trajectory_generation(...
    arm_pos_0(1), arm_speed_0(1), ...  % pos__t_0, speed__t_0
    1, 0, ...  % pos__t_fin, speed__t_fin
    TOTAL_TIME, 3, 3, ...  % time: total, accel, decel
    DT, ...  % dt
    0.15 ...  % av <0.1;.33>
    );

[time2, pos2, speed2, acceleration2] = my_trajectory_generation(...
    arm_pos_0(2), arm_speed_0(2), ...  % pos__t_0, speed__t_0
    1, 0, ...  % pos__t_fin, speed__t_fin
    TOTAL_TIME, 2, 4, ...  % time: total, accel, decel
    DT, ...  % dt
    0.05 ...  % v <0.1;.33>
    );

assert(prod(time1 == time2));

[moments] = run_inverse_dynamic_2DoF_calculate_moment(...
    arm_mass, arm_length, ...
    [pos1'; pos2'], ...
    [speed1'; speed2'], ...
    [acceleration1'; acceleration2']);


%% 1.
[arm_orientation, arm_speed_rad, ...
    sat_position, sat_orientation, ...
    ee_pos, ee_orientation] = dynamic_2DoF_with_free_base(...
    satellite_mass, satellite_inertia, ...
    arm_mass, arm_length, moments, DT, ...
    arm_pos_0, arm_speed_0);
