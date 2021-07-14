# Calculation optimizations.
Test includes different optimization methods for multiplying matrixes using naive and blocked algorithm.
Optimizations:
- SSE, AXV, FMA
- unroll of the loop (2, 4, 8, 16 and 32 times) for every type algorithm and every optimization.

## Configuration
For blocked algorithm, define first and number of subblocks:<br>
#define START_NB 4  // has to be power of 2; 4 is a minimum for AXV<br>
#define N_OF_NB 6<br>

For all algorithms set start n (matrix size = n\*n), next tested n = n*2 <br>
// Range [start, end)<br>
#define START_N 128  // <br>
#define END_N 2048 + 1  // HAS TO BE AT LEAST ONE MORE THAN MAX POWER OF 2 YOU WANT<br>

#define TEST_RESULTS 1  // Flag - whether test results for correctness.<br>
#define NUMBER_OF_TESTS_PER_ALG 5<br>

// Algorithm will be no longer tested, when time passes this threshold<br>
// (for blocked algorithm when time for nb exceed this value - next nb will not be tested)<br>
#define MAX_TIME_OF_SINGLE_ALGORITHM_TEST 100<br>

## Results
Results and stages are printed on the screen. GFLOPS values are saved to the file results.txt, format: val | val | ...<br>
Time below 0.00001s being registered as 0 and saved to file as 1000 (GFLOPS).<br>
Default time equals 1000, so anything over this time will be registered as 1000.<br>
Test skipped due to exceeded time will be printed as 999 and will not be saved to the file, but space and | will be added.

## How to run.
Tests require linux for time receiving lib (time.c / time.h).br>
To run add priviledge to run:<br>
chmod +x make_c_and_run.sh<br><br>
then start:<br><br>
./[make_c_and_run.sh](make_c_and_run.sh)<br>
