function [time, solution_1, solution_2] = ...
    run_indirect_2DoF_on_trajectory(time, pos, arm)

    arm1 = arm(1);
    arm2 = arm(2);

    iterations = size(time, 1);

    solution_1 = zeros(iterations, 2);
    solution_2 = zeros(iterations, 2);

    for i = 1:iterations
        [solution_1(i, :), solution_2(i, :)] = ...
            inverse_2DoF(pos(i, :)', arm1, arm2);
    end
end