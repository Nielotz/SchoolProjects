function [ang_accel] = caluclate_ang_accel(u, m, l)
    moment_of_inertia = m * l * l / 3;
    ang_accel = u / moment_of_inertia;
end
