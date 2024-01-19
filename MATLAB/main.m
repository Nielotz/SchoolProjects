[time1, pos1, speed1, acceleration1] = my_trajectory_generation(...
    0, 0, ...  % pos__t_0, speed__t_0
    1, 0, ...  % pos__t_fin, speed__t_fin
    10, 3, 3, ...  % time: total, accel, decel
    1, ...  % dt
    0.15 ...  % av <0.1;.33>
    );
trajectory1 = [time1, pos1, speed1, acceleration1]
[time2, pos2, speed2, acceleration2] = my_trajectory_generation(...
    0, 0, ...  % pos__t_0, speed__t_0
    1, 0, ...  % pos__t_fin, speed__t_fin
    10, 2, 4, ...  % time: total, accel, decel
    1, ...  % dt
    0.05 ...  % v <0.1;.33>
    );
trajectory2 = [time2, pos2, speed2, acceleration2]

subplot(1, 2, 1)
plot(time1, pos1)
hold on
plot(time2, pos2)
title("Pozycja [m]")
legend("przegub 1", "przegub 2")


subplot(1, 2, 2)
plot(time1, speed1)
hold on
plot(time2, speed2)
title("Prędkość [m/s]")
legend("przegub 1", "przegub 2")
