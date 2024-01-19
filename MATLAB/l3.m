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

subplot(2, 3, 1)
plot(time1, pos1)
hold on
plot(time2, pos2)
title("Wygenerowana trajektoria - pozycja ramion")
legend("przegub 1", "przegub 2")
xlabel("Czas [s]")
ylabel("pozycja [rad]")

subplot(2, 3, 2)
plot(time1, speed1)
hold on
plot(time2, speed2)
title("Wygenerowana trajektoria - prędkość ramion")
legend("przegub 1", "przegub 2")
xlabel("Czas [s]")
ylabel("prędkość [rad/s]")

subplot(2, 3, 3)
plot(time1, acceleration1)
hold on
plot(time2, acceleration2)
title("Wygenerowana trajektoria - przyśpieszenie ramion")
legend("przegub 1", "przegub 2")
xlabel("Czas [s]")
ylabel("przyśpieszenie [rad^2/s]")

%% 1. Moments.
[moments] = run_inverse_dynamic_2DoF_calculate_moment(...
    arm_mass, arm_length, ...
    [pos1'; pos2'], ...
    [speed1'; speed2'], ...
    [acceleration1'; acceleration2']);

%% 2. Confirm results.
[position, speed] = dynamic_2DoF(arm_mass, arm_length, moments, ...
    arm_pos_0, arm_speed_0, DT);

% Plots.
subplot(2, 3, 4)
plot(time1', position(1, :))
hold on
plot(time2, position(2, :))
title("Dynamika prosta - Pozycja")
legend("przegub 1", "przegub 2")
xlabel("Czas [s]")
ylabel("pozycja [rad]")

subplot(2, 3, 5)
plot(time1', speed(1, :))
hold on
plot(time2, speed(2, :))
title("Dynamika prosta - prędkość")
legend("przegub 1", "przegub 2")
xlabel("Czas [s]")
ylabel("prędkość [rad/s]")


subplot(2, 3, 6)
plot(time1', moments(1, :))
hold on
plot(time2, moments(2, :))
title("Dynamika - momenty [rad]")
legend("przegub 1", "przegub 2")
xlabel("Czas [s]")
ylabel("moment")
