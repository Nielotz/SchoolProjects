%% Trajectory generatrion.
[time1, pos1, speed1, acceleration1] = my_trajectory_generation(...
    0, 0, ...  % pos__t_0, speed__t_0
    1, 0, ...  % pos__t_fin, speed__t_fin
    8, 3, 3, ...  % time: total, accel, decel
    1, ...  % dt
    0.15 ...  % av <0.1;.33>
    );

trajectory1 = [time1, pos1, speed1, acceleration1]
[time2, pos2, speed2, acceleration2] = my_trajectory_generation(...
    0, 0, ...  % pos__t_0, speed__t_0
    1, 0, ...  % pos__t_fin, speed__t_fin
    8, 2, 4, ...  % time: total, accel, decel
    1, ...  % dt
    0.05 ...  % v <0.1;.33>
    );
trajectory2 = [time2, pos2, speed2, acceleration2]

%% Direct 2DoF.
arm = [0.6; ...
       0.5];
[time, position, orientation, lin_speed, rad_speed] = ...
    run_direct_2DoF_on_trajectory(time1, [pos1 pos2], [speed1 speed2], arm);

subplot(2, 2, 1)
plot(time, position(:, 1))
hold on
plot(time, position(:, 2))
title("Pozycja [m]")
legend("x", "y")

subplot(2, 2, 2)
plot(time, orientation)
title("Orientacja [rad]")

subplot(2, 2, 3)
plot(time, lin_speed(:, 1))
hold on
plot(time, lin_speed(:, 2))
title("Prędkość liniowa [m/s]")
legend("x", "y")

subplot(2, 2, 4)
plot(time, rad_speed)
title("Prędkość kątowa [rad]")

%% Indirect 2DoF
[time, solution_1, solution_2] = ...
    run_indirect_2DoF_on_trajectory(time, position, arm);

subplot(3, 1, 1)
plot(time, position(:, 1))
hold on
plot(time, position(:, 2))
title("Pozycja [m]")
legend("x", "y")

subplot(3, 1, 2)
plot(time, solution_1(:, 1))
hold on
plot(time, solution_1(:, 2))
title("Pozycja kątowa [rad] - sol 1")
legend("przegub 1", "przegub 2")

subplot(3, 1, 3)
plot(time, solution_2(:, 1))
hold on
plot(time, solution_2(:, 2))
title("Pozycja kątowa [rad] - sol 2")
legend("przegub 1", "przegub 2")
