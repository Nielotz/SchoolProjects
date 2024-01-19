% S_ - Symbolic
% R_ - Return

function [R_time, R_pos, R_speed, R_accel] = ...
    my_trajectory_generation(pos__t_0, speed__t_0, ...
    pos__t_fin, speed__t_fin, ...
    t_fin, t_I_II, deceleration_time, ...
    dt, v)
t_0 = 0;

sim_iter_num = floor((t_fin - t_0) / dt);

% Initialize return values.
R_time = zeros(sim_iter_num, 1);
R_pos = zeros(sim_iter_num, 1);
R_speed = zeros(sim_iter_num, 1);
R_accel = zeros(sim_iter_num, 1);

%%
syms S_t

% First phase equations - acceleration.
syms S_a0 S_a1 S_a2 S_a3
S_I_pos = S_a0 + S_a1*S_t + S_a2*S_t^2 + S_a3*S_t^3;
S_I_speed = S_a1 + 2*S_a2*S_t + 3*S_a3*S_t^2;
S_I_accel = 2*S_a2 + 6*S_a3 * S_t;

% Second phase equations - const speed.
syms S_b0 S_b1
S_II_pos = S_b0 + S_b1*S_t;
S_II_speed = S_b1;
S_II_accel = 0;

% Third phase equations - deceleration.
syms S_c0 S_c1 S_c2 S_c3
S_III_pos = S_c0 + S_c1*S_t + S_c2*S_t^2 + S_c3*S_t^3;
S_III_speed = S_c1 + 2*S_c2*S_t + 3*S_c3*S_t^2;
S_III_accel = 2*S_c2 + 6*S_c3*S_t;

%% Equations with specific S_t.
% t_0
S_I_pos__t_0 = subs(S_I_pos, S_t, t_0);
S_I_speed__t_0 = subs(S_I_speed, S_t, t_0);

% Transition between I and II phase (t_I_II).
S_I_pos__t_I_II = subs(S_I_pos, S_t, t_I_II);
S_I_speed__t_I_II = subs(S_I_speed, S_t, t_I_II);

S_II_pos__t_I_II = subs(S_II_pos, S_t, t_I_II);
S_II_speed__t_I_II = subs(S_II_speed, S_t, t_I_II);

% Transition between II and III phase (t_II_III).
t_II_III = t_fin - deceleration_time;
S_II_pos__t_II_III = subs(S_II_pos, S_t, t_II_III);
S_II_speed__t_II_III = subs(S_II_speed, S_t, t_II_III);

S_III_pos__t_II_III = subs(S_III_pos, S_t, t_II_III);
S_III_speed__t_II_III = subs(S_III_speed, S_t, t_II_III);

% t_fin
S_III_pos__t_fin = subs(S_III_pos, S_t, t_fin);
S_III_speed__t_fin = subs(S_III_speed, S_t, t_fin);

%% Solve for S_a0 S_a1 S_a2 S_a3 S_b0 S_b1 S_c0 S_c1 S_c2 S_c3.
Sstruct_trajectory_coefficients = solve(...
    ...  % Transition at t_0
    S_I_pos__t_0 == pos__t_0, ...
    S_I_speed__t_0 == speed__t_0, ...
    ...  % Transition at t_I_II
    S_I_pos__t_I_II == S_II_pos__t_I_II, ...
    S_I_speed__t_I_II == S_II_speed__t_I_II, ...
    ...  % Transition at t_II_III
    S_II_pos__t_II_III == S_III_pos__t_II_III, ...
    S_II_speed__t_II_III == S_III_speed__t_II_III, ...
    ...  % Transition at t_fin
    S_III_pos__t_fin == pos__t_fin, ...
    S_III_speed__t_fin == speed__t_fin, ...
    ...  % Second phase constant speed.
    S_II_speed == v, ...
    S_I_speed__t_I_II == S_III_speed__t_II_III, ...
    ...% S_I_speed__t_I_II == v == 0, ...
    ...% S_III_speed__t_II_III == v == 0, ...
    ...
    [S_a0 S_a1 S_a2 S_a3 S_b0 S_b1 S_c0 S_c1 S_c2 S_c3] ...
    );

%% Subs calculated coefficients == make equations only t dependent.
stc = Sstruct_trajectory_coefficients;
trajectory_coefficients = [stc.S_a0 stc.S_a1 stc.S_a2 stc.S_a3 ...
    stc.S_b0 stc.S_b1 ...
    stc.S_c0 stc.S_c1 stc.S_c2 stc.S_c3];

S_tc = [S_a0 S_a1 S_a2 S_a3 S_b0 S_b1 S_c0 S_c1 S_c2 S_c3];

S_I_pos_by_t = subs(S_I_pos, S_tc, trajectory_coefficients);
S_II_pos_by_t = subs(S_II_pos, S_tc, trajectory_coefficients);
S_III_pos_by_t = subs(S_III_pos, S_tc, trajectory_coefficients);

S_I_speed_by_t = subs(S_I_speed, S_tc, trajectory_coefficients);
S_II_speed_by_t = subs(S_II_speed, S_tc, trajectory_coefficients);
S_III_speed_by_t = subs(S_III_speed, S_tc, trajectory_coefficients);

S_I_accel_by_t = subs(S_I_accel, S_tc, trajectory_coefficients);
S_II_accel_by_t = subs(S_II_accel, S_tc, trajectory_coefficients);
S_III_accel_by_t = subs(S_III_accel, S_tc, trajectory_coefficients);

%% Prepare data for each step.  "- dt" - compensate closed range.
step_time = [t_0           t_I_II          t_II_III;
             t_I_II - dt   t_II_III - dt   t_fin];
step_syms = [S_I_pos_by_t   S_II_pos_by_t   S_III_pos_by_t;
             S_I_speed_by_t S_II_speed_by_t S_III_speed_by_t;
             S_I_accel_by_t S_II_accel_by_t S_III_accel_by_t];

%% Perform simulation.
sim_iter = 1;
for step_idx = 1:3  % size(step_data, 2)
    t_start = step_time(1, step_idx);
    t_end = step_time(2, step_idx);
    % Ensure has S_t argument:
    
    f_S_pos(S_t) = step_syms(1, step_idx);
    f_S_speed(S_t) = step_syms(2, step_idx);
    f_S_accel(S_t) = step_syms(3, step_idx);

    f_pos =  matlabFunction(f_S_pos);
    f_speed =  matlabFunction(f_S_speed);
    f_accel =  matlabFunction(f_S_accel);

    for t_val = t_start:dt:t_end
        R_time(sim_iter) = (sim_iter - 1) * dt;

        R_pos(sim_iter) = f_pos(t_val);
        R_speed(sim_iter) = f_speed(t_val);
        R_accel(sim_iter) = f_accel(t_val);

        sim_iter = sim_iter + 1;
    end
end
end
