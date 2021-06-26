#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <x86intrin.h>

#include "eval_time.h"

// Range [start, end)
#define START_NB 4 // [ , has to be power of 2, 4 is a minimum for avx
#define N_OF_NB 6 // )

// Range [start, end)
#define START_N 128  // [
#define END_N 2048 + 1  // ) HAS TO BE AT LEAST ONE MORE THAN MAX POWER OF 2 YOU WANT

#define TEST_RESULTS 1  // Flag - whether test results.
#define NUMBER_OF_TESTS_PER_ALG 5

#define MAX_TIME_OF_SINGLE_ALGORITHM_TEST 100  // Algorithm  will be no longer tested, when time passes this threshold.

#define SIZE (unsigned long)(END_N - 1)

// Calculation data holders.
static double a_transposed[SIZE * SIZE];  
static double a_normal[SIZE * SIZE];
static double b[SIZE * SIZE];

// Results holders.
static int is_result1_set = 0;
static double result1[SIZE * SIZE];
static double result2[SIZE * SIZE];

// Naive.
void dgemm_naive(const int, const double* const A, const double* const B, double* const C);

void dgemm_naive_unroll4(const int, const double* const A, const double* const B, double* const C);
void dgemm_naive_unroll16(const int, const double* const A, const double* const B, double* const C);

void dgemm_naive_blocked(const int, const double* const A, const double* const B, double* const C);
void _block_naive(int, int, int, int, const double* const A, const double* const B, double* const C);

// Naive transposed.
void dgemm_naiveT(const int, const double* const A, const double* const B, double* const C);

void dgemm_naiveT_unroll4(const int, const double* const A, const double* const B, double* const C);
void dgemm_naiveT_unroll16(const int, const double* const A, const double* const B, double* const C);

void dgemm_naiveT_blocked(const int, const double* const A, const double* const B, double* const C);
void _block_naiveT(int, int, int, int, const double* const A, const double* const B, double* const C);

// SSE
void dgemm_sse(const int, const double* const A, const double* const B, double* const C);

void dgemm_sse_unroll2(const int n, const double* const A, const double* const B, double* const C);
void dgemm_sse_unroll4(const int n, const double* const A, const double* const B, double* const C);
void dgemm_sse_unroll8(const int n, const double* const A, const double* const B, double* const C);
void dgemm_sse_unroll16(const int n, const double* const A, const double* const B, double* const C);
void dgemm_sse_unroll32(const int n, const double* const A, const double* const B, double* const C);

void dgemm_sse_blocked(const int n, const double* const A, const double* const B, double* const C);
void _block_sse(int, int, int, int, const double* const A, const double* const B, double* const C);

// AVX
void dgemm_avx(const int, const double* const A, const double* const B, double* const C);

void dgemm_avx_unroll2(const int n, const double* const A, const double* const B, double* const C);
void dgemm_avx_unroll4(const int n, const double* const A, const double* const B, double* const C);
void dgemm_avx_unroll8(const int n, const double* const A, const double* const B, double* const C);
void dgemm_avx_unroll16(const int n, const double* const A, const double* const B, double* const C);
void dgemm_avx_unroll32(const int n, const double* const A, const double* const B, double* const C);

void dgemm_avx_blocked(const int, const double* const A, const double* const B, double* const C);
void _block_avx(int, int, int, int, const double* const A, const double* const B, double* const C);

// FMA
void dgemm_avx_fma(const int, const double* const A, const double* const B, double* const C);

void dgemm_avx_fma_unroll2(const int, const double* const A, const double* const B, double* const C);
void dgemm_avx_fma_unroll4(const int, const double* const A, const double* const B, double* const C);
void dgemm_avx_fma_unroll8(const int, const double* const A, const double* const B, double* const C);
void dgemm_avx_fma_unroll16(const int, const double* const A, const double* const B, double* const C);
void dgemm_avx_fma_unroll32(const int, const double* const A, const double* const B, double* const C);

void dgemm_avx_fma_blocked(const int, const double* const A, const double* const B, double* const C);
void _block_avx_fma(int, int, int, int, const double* const A, const double* const B, double* const C);

// Shared
void _block_core(const int n, const double* const A, const double* const B, double* const C, void (*block_algorythm)());

void print_with_gflops(const char* const name, const double time);

void init_arrays(const int n);

// Return non zero on error.
int test_results(const int n, const double* const a, const double* const b);

int n = 0;
int nb = START_NB;
unsigned long f;

typedef struct Algorithm
{
    const void (*f)();
    const char* name;
    int is_transposed;
    double best_time[N_OF_NB];
} Algorithm;

static Algorithm ALGORITHMS[] = {
    { dgemm_sse, "sse", 0 },
    { dgemm_avx, "avx", 0 },
    { dgemm_avx_fma, "avx_fma", 0 },
    { dgemm_naive, "naive", 0 },
    { dgemm_naiveT, "naiveT", 1 },

    { dgemm_sse_unroll2, "sse_unroll2", 0 },
    { dgemm_avx_unroll2, "avx_unroll2", 0 },
    { dgemm_avx_fma_unroll2, "avx_fma_unroll2", 0 },
    
    { dgemm_sse_unroll4, "sse_unroll4", 0 },
    { dgemm_avx_unroll4, "avx_unroll4", 0 },
    { dgemm_avx_fma_unroll4, "avx_fma_unroll4", 0 },
    { dgemm_naive_unroll4, "naive_unroll4", 0 },
    { dgemm_naiveT_unroll4, "naiveT_unroll4", 1 },
    
    { dgemm_sse_unroll8, "sse_unroll8", 0 },
    { dgemm_avx_unroll8, "avx_unroll8", 0 },
    { dgemm_avx_fma_unroll8, "avx_fma_unroll8", 0 },
    
    { dgemm_sse_unroll16, "sse_unroll16", 0 },
    { dgemm_avx_unroll16, "avx_unroll16", 0 },
    { dgemm_avx_fma_unroll16, "avx_fma_unroll16", 0 },
    { dgemm_naive_unroll16, "naive_unroll16", 0 },
    { dgemm_naiveT_unroll16, "naiveT_unroll16", 1 },
    
    { dgemm_sse_unroll32, "sse_unroll32", 0 },
    { dgemm_avx_unroll32, "avx_unroll32", 0 },
    { dgemm_avx_fma_unroll32, "avx_fma_unroll32", 0 },  
};

static Algorithm BLOCKED_ALGORITHMS[] = {
    { dgemm_naive_blocked, "naive_blocked", 0 },
    { dgemm_naiveT_blocked, "naiveT_blocked", 1 },
    { dgemm_sse_blocked, "sse_blocked", 0 },
    { dgemm_avx_blocked, "avx_blocked", 0 },
    { dgemm_avx_fma_blocked, "avx_fma_blocked", 0 },
        
};

#define ALGORITHMS_LEN ((sizeof(ALGORITHMS)/sizeof(0[ALGORITHMS])) / ((size_t)(!(sizeof(ALGORITHMS) % sizeof(0[ALGORITHMS])))))
#define BLOCKED_ALGORITHMS_LEN ((sizeof(BLOCKED_ALGORITHMS)/sizeof(0[BLOCKED_ALGORITHMS])) / ((size_t)(!(sizeof(BLOCKED_ALGORITHMS) % sizeof(0[BLOCKED_ALGORITHMS])))))

void test_algorithm(Algorithm* const algorithm, const int n, double* const results, const int best_time_idx);

void results_print_headers(FILE* const output);
void results_print_current_results(FILE* const output);


int main(void)
{
    FILE* algorithms_results = fopen("results.txt", "a+");
    results_print_headers(algorithms_results);

    for (n = START_N; n < END_N; n <<= 1)
    {
        printf("Test %d/%d, n: %d\n", (int)(n / START_N), (int)log2f((float)((END_N-1) / START_N)), n);
        fflush(stdout);
        init_arrays(n);
        is_result1_set = 0;
        // Amount of floating point operations.
        f = 2 * (unsigned long)n * (unsigned long)n * (unsigned long)n;           

        for (int idx = 0; idx < ALGORITHMS_LEN; idx++)
            ALGORITHMS[idx].best_time[0] = 1000;

        // Blocked algorithms.
        for (int idx = 0; idx < BLOCKED_ALGORITHMS_LEN; idx++)
            for (int idx_of_nb = 0; idx_of_nb < N_OF_NB; idx_of_nb++)
                BLOCKED_ALGORITHMS[idx].best_time[idx_of_nb] = 1000;

        Algorithm* algorithm = &ALGORITHMS[0];
        for (int iteration = 0; iteration < NUMBER_OF_TESTS_PER_ALG; iteration++)
        {
            printf("\tIteration %d/%d, testing algorithms: %d | ", 
                iteration + 1, NUMBER_OF_TESTS_PER_ALG, ALGORITHMS_LEN + BLOCKED_ALGORITHMS_LEN);

            for (int idx = 0; idx < ALGORITHMS_LEN; idx++)
            {
                printf("%d ", idx + 1);
                fflush(stdout);

                algorithm = &ALGORITHMS[idx];
                if (TEST_RESULTS && !is_result1_set)
                {
                    test_algorithm(algorithm, n, result1, 0); 
                    is_result1_set = 1;
                    continue;
                }

                if (algorithm->best_time[0] < 0.00001
                    || algorithm->best_time[0] > MAX_TIME_OF_SINGLE_ALGORITHM_TEST
                    && abs(algorithm->best_time[0] - 1000.) > 0.000001)
                        continue;
                test_algorithm(algorithm, n, result2, 0);
                
                if (TEST_RESULTS && test_results(n, result1, result2))
                {
                    printf("\nError: %s do not match %s!\n", ALGORITHMS[0].name, algorithm->name);
                    fflush(stdout);
                    exit(1);
                }
            }

            nb = START_NB;
            for (register int idx_of_nb = 0; idx_of_nb < N_OF_NB; idx_of_nb++)
            {
                printf("\n\t\tnb: %d/%d, test: %d | ",
                    idx_of_nb + 1, N_OF_NB, BLOCKED_ALGORITHMS_LEN);

                for (int idx = 0; idx < BLOCKED_ALGORITHMS_LEN; idx++)
                {
                    printf("%d ", idx + 1);
                    fflush(stdout);

                    algorithm = &BLOCKED_ALGORITHMS[idx];
                    if (TEST_RESULTS && !is_result1_set)  // Required for validating output.
                    {
                        test_algorithm(algorithm, n, result1, idx_of_nb); 
                        is_result1_set = 1;
                        continue;
                    }

                    // When previous nb exceeded time.
                    if (idx_of_nb != 0 && algorithm->best_time[idx_of_nb - 1] > MAX_TIME_OF_SINGLE_ALGORITHM_TEST)
                    {
                        if (abs(algorithm->best_time[idx_of_nb] - 1000.) < 0.000001)
                            algorithm->best_time[idx_of_nb] = 999.;
                        continue;
                    }

                    // Check does best time of this time exceeded boundaries. 
                    if (algorithm->best_time[idx_of_nb] < 0.00001 
                        || algorithm->best_time[idx_of_nb] > MAX_TIME_OF_SINGLE_ALGORITHM_TEST
                        && abs(algorithm->best_time[idx_of_nb] - 1000.) > 0.000001)
                        continue;

                    test_algorithm(algorithm, n, result2, idx_of_nb);
                    
                    if (TEST_RESULTS && test_results(n, result1, result2))
                    {
                        if (ALGORITHMS_LEN)
                            printf("\nError: %s do not match %s!\n", ALGORITHMS[0].name, algorithm->name);
                        else
                            printf("\nError: %s do not match %s!\n", BLOCKED_ALGORITHMS[0].name, algorithm->name);
                        fflush(stdout);
                        exit(1);
                    }
                }
                nb <<= 1;
            }
            printf("\n");
        }

        results_print_current_results(algorithms_results);

        // Print results.
        printf("Best results for n = %d where f = %lu:\n", n, f);
        for (int idx = 0; idx < ALGORITHMS_LEN; idx++)
            print_with_gflops(ALGORITHMS[idx].name, ALGORITHMS[idx].best_time[0]);

        for (int idx = 0; idx < BLOCKED_ALGORITHMS_LEN; idx++)
        {
            for (int i = 0; i < N_OF_NB; i++)
            {
                if (START_NB << i > n)
                    break;
    
                printf("\t%s for nb = %3d", BLOCKED_ALGORITHMS[idx].name, START_NB << i);
                print_with_gflops(":", BLOCKED_ALGORITHMS[idx].best_time[i]);
            }
        }
        printf("\n");
        fflush(stdout);
    }
    fprintf(algorithms_results, "\n");
    fflush(algorithms_results);
    fclose(algorithms_results);
    return 0;
}

void results_print_headers(FILE* const output)
{
    fprintf(output, "%-3s  ", "n");

    for (int alg_idx = 0; alg_idx < ALGORITHMS_LEN; alg_idx++)
        fprintf(output, "| %s ", ALGORITHMS[alg_idx].name);
    
    fprintf(output, "| ");

    for (int alg_idx = 0; alg_idx < BLOCKED_ALGORITHMS_LEN; alg_idx++)
    {
        fprintf(output, "| %s, nb: ", BLOCKED_ALGORITHMS[alg_idx].name);
        for (int nb_idx = 0; nb_idx < N_OF_NB; nb_idx++)
            fprintf(output, "%d | ", START_NB << nb_idx);
    }
    
    fprintf(output, "|\n");
}

void results_print_current_results(FILE* const output)
{
    fprintf(output, "%-4d ", n);
    
    for (int alg_idx = 0; alg_idx < ALGORITHMS_LEN; alg_idx++)
    {
        if (ALGORITHMS[alg_idx].best_time[0] < 0.00001)
            fprintf(output, "| %.5lf ", 
                ALGORITHMS[alg_idx].name, 1000.);
        else
            fprintf(output, "| %.5lf ", 
                ALGORITHMS[alg_idx].name, (double)f / ALGORITHMS[alg_idx].best_time[0] * 1.0e-9);
    }

    for (int alg_idx = 0; alg_idx < BLOCKED_ALGORITHMS_LEN; alg_idx++)
    {
        fprintf(output, "|");
        for (int nb_idx = 0; nb_idx < N_OF_NB; nb_idx++)
            if (START_NB << nb_idx > n)
                fprintf(output, " | ");
            else
            {
                if (abs(BLOCKED_ALGORITHMS[alg_idx].best_time[nb_idx] - 999.0) < 3)
                    fprintf(output, "| ");
                else if (BLOCKED_ALGORITHMS[alg_idx].best_time[nb_idx] < 0.00001)
                    fprintf(output, "| %.5lf ", 1000.);
                else
                    fprintf(output, "| %.5lf ", (double)f / BLOCKED_ALGORITHMS[alg_idx].best_time[nb_idx] * 1.0e-9);
            }
    }

    fprintf(output, "|\n");
    fflush(output);
}

void init_arrays(const int n)
{
    for (long i = 0; i < n; ++i)
        for (long j = 0; j < n; ++j)
        {
            a_normal[j + i*n] = a_transposed[i + j*n] = (double)(i + j*n);
            b[j + i*n] = (double)(j + i*n);
        }
}

int test_results(const int n, const double* const a, const double* const b)
{
    for (long i = 0; i < n; ++i) 
        if (fabs(a[i] - b[i]) > 1.0e-9)
        {
            printf("Different idx: %d\n", i);
            for (int i = 0; i < 10; i++) 
                printf("%10.lf ", a[i]);
            printf("\n");
            for (int i = 0; i < 10; i++) 
                printf("%10.lf ", b[i]);
            return 1;
        }
    return 0;
}

void print_with_gflops(const char* const name, const double time)
{
    printf("\t%-15s: %lfs, GFLOPS:%lf\n", name, time, (double)f / time * 1.0e-9);
}

void test_algorithm(Algorithm* const algorithm, const int n, double* const results, const int best_time_idx)
{
    memset(results, 0, n * sizeof(double));  // Zero results.
    
    // Prepare appropriate data.
    const double* a;
    if (algorithm->is_transposed)
        a = a_transposed;
    else
        a = a_normal;

    // Test algorithm.
    init_time();
    algorithm->f(n, a, b, results);
    double time = read_time();
    
    // Update best time.
    if (algorithm->best_time[best_time_idx] > time)
        algorithm->best_time[best_time_idx] = time;
}

void _block_core(const int n, const double* const A, const double* const B, double* const C, void (*block_algorythm)())
{
    for (register int bi = 0; bi < n; bi += nb)
        for (register int bj = 0; bj < n; bj += nb)
            for (register int bk = 0; bk < n; bk += nb)
                block_algorythm(n, bi, bj, bk, A, B, C);
}

// Naive.
void dgemm_naive_blocked(const int n, const double* const A, const double* const B, double* const C)
{
    _block_core(n, A, B, C, _block_naive);
}

void _block_naive(const int n, const int const bi, const int bj, const int bk, const double* const A, const double* const B, double* const C)
{
    for (register int i = bi; i < bi + nb; i++)
        for (register int j = bj; j < bj + nb; j++)
        {
            register double cij = C[i + j*n];
            for (register int k = bk; k < bk + nb; k++)
                cij += A[i + k*n] * B[k + j*n];
            C[i + j*n] = cij;
        }
}

// Naive transposed.
void dgemm_naiveT_blocked(const int n, const double* const A, const double* const B, double* const C)
{
    _block_core(n, A, B, C, _block_naiveT);
}

void _block_naiveT(const int n, const int const bi, const int bj, const int bk, const double* const A, const double* const B, double* const C)
{
    for (register int i = bi; i < bi + nb; i++)
        for (register int j = bj; j < bj + nb; j++)
        {
            register double cij = C[i + j*n];
            for (register int k = bk; k < bk + nb; k++)
                cij += A[k + i*n] * B[k + j*n];
            C[i + j*n] = cij;
        }
}

// Naive
void dgemm_naive(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i++)
        for (register int j = 0; j < n; j++)
        {       
            register double reg0 = A[i * n] * B[j * n];
            for (register int k = 1; k < n; k++)
                reg0  += A[i + k*n] * B[k + j*n];
            C[i + j*n] += reg0;
        }
}

void dgemm_naive_unroll4(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 2)
        for (register int j = 0; j < n; j += 2)
        {       
            register double reg0 = A[0 + i] * B[(j + 0) * n];
            register double reg1 = A[1 + i] * B[(j + 0) * n];
            register double reg4 = A[0 + i] * B[(j + 1) * n];
            register double reg5 = A[1 + i] * B[(j + 1) * n];
            for (register int k = 1; k < n; k++)

            {
                reg0  += A[0 + i + k*n] * B[k + (j+0)*n];
                reg1  += A[1 + i + k*n] * B[k + (j+0)*n];
                reg4  += A[0 + i + k*n] * B[k + (j+1)*n];
                reg5  += A[1 + i + k*n] * B[k + (j+1)*n];
            }

            C[i +         j*n] += reg0;
            C[i + 1 +     j*n] += reg1;
            C[i +     (j+1)*n] += reg4;
            C[i + 1 + (j+1)*n] += reg5;
        }
}

void dgemm_naive_unroll16(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 4)
        for (register int j = 0; j < n; j += 4)
        {       
            register double reg0  = A[0 + i] * B[(j + 0) * n];
            register double reg1  = A[1 + i] * B[(j + 0) * n];
            register double reg2  = A[2 + i] * B[(j + 0) * n];
            register double reg3  = A[3 + i] * B[(j + 0) * n];
            register double reg4  = A[0 + i] * B[(j + 1) * n];
            register double reg5  = A[1 + i] * B[(j + 1) * n];
            register double reg6  = A[2 + i] * B[(j + 1) * n];
            register double reg7  = A[3 + i] * B[(j + 1) * n];
            register double reg8  = A[0 + i] * B[(j + 2) * n];
            register double reg9  = A[1 + i] * B[(j + 2) * n];
            register double reg10 = A[2 + i] * B[(j + 2) * n];
            register double reg11 = A[3 + i] * B[(j + 2) * n];
            register double reg12 = A[0 + i] * B[(j + 3) * n];
            register double reg13 = A[1 + i] * B[(j + 3) * n];
            register double reg14 = A[2 + i] * B[(j + 3) * n];
            register double reg15 = A[3 + i] * B[(j + 3) * n];

            for (register int k = 1; k < n; k++)
            {
                reg0  += A[(0 + i) + k*n] * B[k + (j+0)*n];
                reg1  += A[(1 + i) + k*n] * B[k + (j+0)*n];
                reg2  += A[(2 + i) + k*n] * B[k + (j+0)*n];
                reg3  += A[(3 + i) + k*n] * B[k + (j+0)*n];
                reg4  += A[(0 + i) + k*n] * B[k + (j+1)*n];
                reg5  += A[(1 + i) + k*n] * B[k + (j+1)*n];
                reg6  += A[(2 + i) + k*n] * B[k + (j+1)*n];
                reg7  += A[(3 + i) + k*n] * B[k + (j+1)*n];
                reg8  += A[(0 + i) + k*n] * B[k + (j+2)*n];
                reg9  += A[(1 + i) + k*n] * B[k + (j+2)*n];
                reg10 += A[(2 + i) + k*n] * B[k + (j+2)*n];
                reg11 += A[(3 + i) + k*n] * B[k + (j+2)*n];
                reg12 += A[(0 + i) + k*n] * B[k + (j+3)*n];
                reg13 += A[(1 + i) + k*n] * B[k + (j+3)*n];
                reg14 += A[(2 + i) + k*n] * B[k + (j+3)*n];
                reg15 += A[(3 + i) + k*n] * B[k + (j+3)*n];
            }

            C[i +         j*n] += reg0;
            C[i + 1 +     j*n] += reg1;
            C[i + 2 +     j*n] += reg2;
            C[i + 3 +     j*n] += reg3;
            C[i +     (j+1)*n] += reg4;
            C[i + 1 + (j+1)*n] += reg5;
            C[i + 2 + (j+1)*n] += reg6;
            C[i + 3 + (j+1)*n] += reg7;
            C[i +     (j+2)*n] += reg8;
            C[i + 1 + (j+2)*n] += reg9;
            C[i + 2 + (j+2)*n] += reg10;
            C[i + 3 + (j+2)*n] += reg11;
            C[i +     (j+3)*n] += reg12;
            C[i + 1 + (j+3)*n] += reg13;
            C[i + 2 + (j+3)*n] += reg14;
            C[i + 3 + (j+3)*n] += reg15;
        }
}

// Naive transposed
void dgemm_naiveT(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i++)
        for (register int j = 0; j < n; j++)
        {       
            register double reg0 = A[i * n] * B[j * n];
            for (register int k = 1; k < n; k++)
                reg0  += A[k + i*n] * B[k + j*n];
            C[i + j*n] += reg0;
        }
}

void dgemm_naiveT_unroll4(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 2)
        for (register int j = 0; j < n; j += 2)
        {       
            register double reg0 = A[(0 + i) * n] * B[(j + 0) * n];
            register double reg1 = A[(1 + i) * n] * B[(j + 0) * n];
            register double reg4 = A[(0 + i) * n] * B[(j + 1) * n];
            register double reg5 = A[(1 + i) * n] * B[(j + 1) * n];

            for (register int k = 1; k < n; k++)
            {
                reg0  += A[k + (0 + i)*n] * B[k + (j+0)*n];
                reg1  += A[k + (1 + i)*n] * B[k + (j+0)*n];
                reg4  += A[k + (0 + i)*n] * B[k + (j+1)*n];
                reg5  += A[k + (1 + i)*n] * B[k + (j+1)*n];
            }

            C[i +         j*n] += reg0;
            C[i + 1 +     j*n] += reg1;
            C[i +     (j+1)*n] += reg4;
            C[i + 1 + (j+1)*n] += reg5;
        }
}

void dgemm_naiveT_unroll16(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 4)
        for (register int j = 0; j < n; j += 4)
        {       
            register double reg0  = A[(0 + i) * n] * B[(j + 0) * n];
            register double reg1  = A[(1 + i) * n] * B[(j + 0) * n];
            register double reg2  = A[(2 + i) * n] * B[(j + 0) * n];
            register double reg3  = A[(3 + i) * n] * B[(j + 0) * n];
            register double reg4  = A[(0 + i) * n] * B[(j + 1) * n];
            register double reg5  = A[(1 + i) * n] * B[(j + 1) * n];
            register double reg6  = A[(2 + i) * n] * B[(j + 1) * n];
            register double reg7  = A[(3 + i) * n] * B[(j + 1) * n];
            register double reg8  = A[(0 + i) * n] * B[(j + 2) * n];
            register double reg9  = A[(1 + i) * n] * B[(j + 2) * n];
            register double reg10 = A[(2 + i) * n] * B[(j + 2) * n];
            register double reg11 = A[(3 + i) * n] * B[(j + 2) * n];
            register double reg12 = A[(0 + i) * n] * B[(j + 3) * n];
            register double reg13 = A[(1 + i) * n] * B[(j + 3) * n];
            register double reg14 = A[(2 + i) * n] * B[(j + 3) * n];
            register double reg15 = A[(3 + i) * n] * B[(j + 3) * n];

            for (register int k = 1; k < n; k++)
            {
                reg0  += A[k + (0 + i)*n] * B[k + (j+0)*n];
                reg1  += A[k + (1 + i)*n] * B[k + (j+0)*n];
                reg2  += A[k + (2 + i)*n] * B[k + (j+0)*n];
                reg3  += A[k + (3 + i)*n] * B[k + (j+0)*n];
                reg4  += A[k + (0 + i)*n] * B[k + (j+1)*n];
                reg5  += A[k + (1 + i)*n] * B[k + (j+1)*n];
                reg6  += A[k + (2 + i)*n] * B[k + (j+1)*n];
                reg7  += A[k + (3 + i)*n] * B[k + (j+1)*n];
                reg8  += A[k + (0 + i)*n] * B[k + (j+2)*n];
                reg9  += A[k + (1 + i)*n] * B[k + (j+2)*n];
                reg10 += A[k + (2 + i)*n] * B[k + (j+2)*n];
                reg11 += A[k + (3 + i)*n] * B[k + (j+2)*n];
                reg12 += A[k + (0 + i)*n] * B[k + (j+3)*n];
                reg13 += A[k + (1 + i)*n] * B[k + (j+3)*n];
                reg14 += A[k + (2 + i)*n] * B[k + (j+3)*n];
                reg15 += A[k + (3 + i)*n] * B[k + (j+3)*n];
            }

            C[i +         j*n] += reg0;
            C[i + 1 +     j*n] += reg1;
            C[i + 2 +     j*n] += reg2;
            C[i + 3 +     j*n] += reg3;
            C[i +     (j+1)*n] += reg4;
            C[i + 1 + (j+1)*n] += reg5;
            C[i + 2 + (j+1)*n] += reg6;
            C[i + 3 + (j+1)*n] += reg7;
            C[i +     (j+2)*n] += reg8;
            C[i + 1 + (j+2)*n] += reg9;
            C[i + 2 + (j+2)*n] += reg10;
            C[i + 3 + (j+2)*n] += reg11;
            C[i +     (j+3)*n] += reg12;
            C[i + 1 + (j+3)*n] += reg13;
            C[i + 2 + (j+3)*n] += reg14;
            C[i + 3 + (j+3)*n] += reg15;
        }
}

// SSE
void dgemm_sse_blocked(const int n, const double* const A, const double* const B, double* const C)
{
    _block_core(n, A, B, C, _block_sse);
}

void _block_sse(const int n, const int const bi, const int bj, const int bk, const double* const A, const double* const B, double* const C)
{
    for (register int i = bi; i < bi + nb; i += 2)
        for (register int j = bj; j < bj + nb; j++)
        {
            __m128d reg = _mm_load_pd(C + i + j*n);
            for (register int k = bk; k < bk + nb; k++)
                reg = _mm_add_pd(reg, _mm_mul_pd(_mm_load_pd(A + n*k + i), _mm_load1_pd(B + k + j*n)));
            _mm_store_pd(C + i + j*n, reg);
        }
}

void dgemm_sse(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 2)
        for (register int j = 0; j < n; j++)
        {
            __m128d reg = _mm_load_pd(C + i + j*n);
            for (register int k = 0; k < n; k++)
                reg = _mm_add_pd(reg, _mm_mul_pd(_mm_load_pd(A + n*k + i), _mm_load1_pd(B + k + j*n)));
            _mm_store_pd(C + i + j*n, reg);
        }
}

void dgemm_sse_unroll2(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 4)
        for (register int j = 0; j < n; j++)
            {
                __m128d reg0 = _mm_load_pd(C + i +     j*n);
                __m128d reg1 = _mm_load_pd(C + i + 2 + j*n);

                for (register int k = 0; k < n; ++k)
                {
                    __m128d bkj = _mm_load1_pd(B + k + j*n);
                    reg0 = _mm_add_pd(reg0, _mm_mul_pd(_mm_load_pd(A + n*k +     i), bkj));
                    reg1 = _mm_add_pd(reg1, _mm_mul_pd(_mm_load_pd(A + n*k + 2 + i), bkj));
                }

                _mm_store_pd(C + i +     j*n, reg0);
                _mm_store_pd(C + i + 2 + j*n, reg1);
            }
}

void dgemm_sse_unroll4(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 8)
        for (register int j = 0; j < n; j++)
        {
            __m128d reg0 = _mm_load_pd(C + i + 0 + j*n);
            __m128d reg1 = _mm_load_pd(C + i + 2 + j*n);
            __m128d reg2 = _mm_load_pd(C + i + 4 + j*n);
            __m128d reg3 = _mm_load_pd(C + i + 6 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m128d bkj = _mm_load1_pd(B + k + j*n);
                reg0 = _mm_add_pd(reg0, _mm_mul_pd(_mm_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm_add_pd(reg1, _mm_mul_pd(_mm_load_pd(A + n*k + 2 + i), bkj));
                reg2 = _mm_add_pd(reg2, _mm_mul_pd(_mm_load_pd(A + n*k + 4 + i), bkj));
                reg3 = _mm_add_pd(reg3, _mm_mul_pd(_mm_load_pd(A + n*k + 6 + i), bkj));
            }

            _mm_store_pd(C + i + 0 + j*n, reg0);
            _mm_store_pd(C + i + 2 + j*n, reg1);
            _mm_store_pd(C + i + 4 + j*n, reg2);
            _mm_store_pd(C + i + 6 + j*n, reg3);
        }
}

void dgemm_sse_unroll8(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 16)
        for (register int j = 0; j < n; j++)
        {
            __m128d reg0 = _mm_load_pd(C + i + 0 + j*n);
            __m128d reg1 = _mm_load_pd(C + i + 2 + j*n);
            __m128d reg2 = _mm_load_pd(C + i + 4 + j*n);
            __m128d reg3 = _mm_load_pd(C + i + 6 + j*n);
            __m128d reg4 = _mm_load_pd(C + i + 8 + j*n);
            __m128d reg5 = _mm_load_pd(C + i + 10 + j*n);
            __m128d reg6 = _mm_load_pd(C + i + 12 + j*n);
            __m128d reg7 = _mm_load_pd(C + i + 14 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m128d bkj = _mm_load1_pd(B + k + j*n);
                reg0 = _mm_add_pd(reg0, _mm_mul_pd(_mm_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm_add_pd(reg1, _mm_mul_pd(_mm_load_pd(A + n*k + 2 + i), bkj));
                reg2 = _mm_add_pd(reg2, _mm_mul_pd(_mm_load_pd(A + n*k + 4 + i), bkj));
                reg3 = _mm_add_pd(reg3, _mm_mul_pd(_mm_load_pd(A + n*k + 6 + i), bkj));
                reg4 = _mm_add_pd(reg4, _mm_mul_pd(_mm_load_pd(A + n*k + 8 + i), bkj));
                reg5 = _mm_add_pd(reg5, _mm_mul_pd(_mm_load_pd(A + n*k + 10 + i), bkj));
                reg6 = _mm_add_pd(reg6, _mm_mul_pd(_mm_load_pd(A + n*k + 12 + i), bkj));
                reg7 = _mm_add_pd(reg7, _mm_mul_pd(_mm_load_pd(A + n*k + 14 + i), bkj));
            }

            _mm_store_pd(C + i + 0 + j*n, reg0);
            _mm_store_pd(C + i + 2 + j*n, reg1);
            _mm_store_pd(C + i + 4 + j*n, reg2);
            _mm_store_pd(C + i + 6 + j*n, reg3);
            _mm_store_pd(C + i + 8 + j*n, reg4);
            _mm_store_pd(C + i + 10 + j*n, reg5);
            _mm_store_pd(C + i + 12 + j*n, reg6);
            _mm_store_pd(C + i + 14 + j*n, reg7);
        }
}

void dgemm_sse_unroll16(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 32)
        for (register int j = 0; j < n; j++)
        {
            __m128d reg0 = _mm_load_pd(C + i + 0 + j*n);
            __m128d reg1 = _mm_load_pd(C + i + 2 + j*n);
            __m128d reg2 = _mm_load_pd(C + i + 4 + j*n);
            __m128d reg3 = _mm_load_pd(C + i + 6 + j*n);
            __m128d reg4 = _mm_load_pd(C + i + 8 + j*n);
            __m128d reg5 = _mm_load_pd(C + i + 10 + j*n);
            __m128d reg6 = _mm_load_pd(C + i + 12 + j*n);
            __m128d reg7 = _mm_load_pd(C + i + 14 + j*n);
            __m128d reg8 = _mm_load_pd(C + i + 16 + j*n);
            __m128d reg9 = _mm_load_pd(C + i + 18 + j*n);
            __m128d reg10 = _mm_load_pd(C + i + 20 + j*n);
            __m128d reg11 = _mm_load_pd(C + i + 22 + j*n);
            __m128d reg12 = _mm_load_pd(C + i + 24 + j*n);
            __m128d reg13 = _mm_load_pd(C + i + 26 + j*n);
            __m128d reg14 = _mm_load_pd(C + i + 28 + j*n);
            __m128d reg15 = _mm_load_pd(C + i + 30 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m128d bkj = _mm_load1_pd(B + k + j*n);
                reg0 = _mm_add_pd(reg0, _mm_mul_pd(_mm_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm_add_pd(reg1, _mm_mul_pd(_mm_load_pd(A + n*k + 2 + i), bkj));
                reg2 = _mm_add_pd(reg2, _mm_mul_pd(_mm_load_pd(A + n*k + 4 + i), bkj));
                reg3 = _mm_add_pd(reg3, _mm_mul_pd(_mm_load_pd(A + n*k + 6 + i), bkj));
                reg4 = _mm_add_pd(reg4, _mm_mul_pd(_mm_load_pd(A + n*k + 8 + i), bkj));
                reg5 = _mm_add_pd(reg5, _mm_mul_pd(_mm_load_pd(A + n*k + 10 + i), bkj));
                reg6 = _mm_add_pd(reg6, _mm_mul_pd(_mm_load_pd(A + n*k + 12 + i), bkj));
                reg7 = _mm_add_pd(reg7, _mm_mul_pd(_mm_load_pd(A + n*k + 14 + i), bkj));
                reg8 = _mm_add_pd(reg8, _mm_mul_pd(_mm_load_pd(A + n*k + 16 + i), bkj));
                reg9 = _mm_add_pd(reg9, _mm_mul_pd(_mm_load_pd(A + n*k + 18 + i), bkj));
                reg10 = _mm_add_pd(reg10, _mm_mul_pd(_mm_load_pd(A + n*k + 20 + i), bkj));
                reg11 = _mm_add_pd(reg11, _mm_mul_pd(_mm_load_pd(A + n*k + 22 + i), bkj));
                reg12 = _mm_add_pd(reg12, _mm_mul_pd(_mm_load_pd(A + n*k + 24 + i), bkj));
                reg13 = _mm_add_pd(reg13, _mm_mul_pd(_mm_load_pd(A + n*k + 26 + i), bkj));
                reg14 = _mm_add_pd(reg14, _mm_mul_pd(_mm_load_pd(A + n*k + 28 + i), bkj));
                reg15 = _mm_add_pd(reg15, _mm_mul_pd(_mm_load_pd(A + n*k + 30 + i), bkj));
            }

            _mm_store_pd(C + i + 0 + j*n, reg0);
            _mm_store_pd(C + i + 2 + j*n, reg1);
            _mm_store_pd(C + i + 4 + j*n, reg2);
            _mm_store_pd(C + i + 6 + j*n, reg3);
            _mm_store_pd(C + i + 8 + j*n, reg4);
            _mm_store_pd(C + i + 10 + j*n, reg5);
            _mm_store_pd(C + i + 12 + j*n, reg6);
            _mm_store_pd(C + i + 14 + j*n, reg7);
            _mm_store_pd(C + i + 16 + j*n, reg8);
            _mm_store_pd(C + i + 18 + j*n, reg9);
            _mm_store_pd(C + i + 20 + j*n, reg10);
            _mm_store_pd(C + i + 22 + j*n, reg11);
            _mm_store_pd(C + i + 24 + j*n, reg12);
            _mm_store_pd(C + i + 26 + j*n, reg13);
            _mm_store_pd(C + i + 28 + j*n, reg14);
            _mm_store_pd(C + i + 30 + j*n, reg15);
        }
}

void dgemm_sse_unroll32(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 64)
        for (register int j = 0; j < n; j++)
        {
            __m128d reg0 = _mm_load_pd(C + i +  0 + j*n);
            __m128d reg1 = _mm_load_pd(C + i +  2 + j*n);
            __m128d reg2 = _mm_load_pd(C + i +  4 + j*n);
            __m128d reg3 = _mm_load_pd(C + i +  6 + j*n);
            __m128d reg4 = _mm_load_pd(C + i +  8 + j*n);
            __m128d reg5 = _mm_load_pd(C + i +  10 + j*n);
            __m128d reg6 = _mm_load_pd(C + i +  12 + j*n);
            __m128d reg7 = _mm_load_pd(C + i +  14 + j*n);
            __m128d reg8 = _mm_load_pd(C + i +  16 + j*n);
            __m128d reg9 = _mm_load_pd(C + i +  18 + j*n);
            __m128d reg10 = _mm_load_pd(C + i +  20 + j*n);
            __m128d reg11 = _mm_load_pd(C + i +  22 + j*n);
            __m128d reg12 = _mm_load_pd(C + i +  24 + j*n);
            __m128d reg13 = _mm_load_pd(C + i +  26 + j*n);
            __m128d reg14 = _mm_load_pd(C + i +  28 + j*n);
            __m128d reg15 = _mm_load_pd(C + i +  30 + j*n);
            __m128d reg16 = _mm_load_pd(C + i +  32 + j*n);
            __m128d reg17 = _mm_load_pd(C + i +  34 + j*n);
            __m128d reg18 = _mm_load_pd(C + i +  36 + j*n);
            __m128d reg19 = _mm_load_pd(C + i +  38 + j*n);
            __m128d reg20 = _mm_load_pd(C + i +  40 + j*n);
            __m128d reg21 = _mm_load_pd(C + i +  42 + j*n);
            __m128d reg22 = _mm_load_pd(C + i +  44 + j*n);
            __m128d reg23 = _mm_load_pd(C + i +  46 + j*n);
            __m128d reg24 = _mm_load_pd(C + i +  48 + j*n);
            __m128d reg25 = _mm_load_pd(C + i +  50 + j*n);
            __m128d reg26 = _mm_load_pd(C + i +  52 + j*n);
            __m128d reg27 = _mm_load_pd(C + i +  54 + j*n);
            __m128d reg28 = _mm_load_pd(C + i +  56 + j*n);
            __m128d reg29 = _mm_load_pd(C + i +  58 + j*n);
            __m128d reg30 = _mm_load_pd(C + i +  60 + j*n);
            __m128d reg31 = _mm_load_pd(C + i +  62 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m128d bkj = _mm_load1_pd(B + k + j*n);
                reg0 = _mm_add_pd(reg0, _mm_mul_pd(_mm_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm_add_pd(reg1, _mm_mul_pd(_mm_load_pd(A + n*k + 2 + i), bkj));
                reg2 = _mm_add_pd(reg2, _mm_mul_pd(_mm_load_pd(A + n*k + 4 + i), bkj));
                reg3 = _mm_add_pd(reg3, _mm_mul_pd(_mm_load_pd(A + n*k + 6 + i), bkj));
                reg4 = _mm_add_pd(reg4, _mm_mul_pd(_mm_load_pd(A + n*k + 8 + i), bkj));
                reg5 = _mm_add_pd(reg5, _mm_mul_pd(_mm_load_pd(A + n*k + 10 + i), bkj));
                reg6 = _mm_add_pd(reg6, _mm_mul_pd(_mm_load_pd(A + n*k + 12 + i), bkj));
                reg7 = _mm_add_pd(reg7, _mm_mul_pd(_mm_load_pd(A + n*k + 14 + i), bkj));
                reg8 = _mm_add_pd(reg8, _mm_mul_pd(_mm_load_pd(A + n*k + 16 + i), bkj));
                reg9 = _mm_add_pd(reg9, _mm_mul_pd(_mm_load_pd(A + n*k + 18 + i), bkj));
                reg10 = _mm_add_pd(reg10, _mm_mul_pd(_mm_load_pd(A + n*k + 20 + i), bkj));
                reg11 = _mm_add_pd(reg11, _mm_mul_pd(_mm_load_pd(A + n*k + 22 + i), bkj));
                reg12 = _mm_add_pd(reg12, _mm_mul_pd(_mm_load_pd(A + n*k + 24 + i), bkj));
                reg13 = _mm_add_pd(reg13, _mm_mul_pd(_mm_load_pd(A + n*k + 26 + i), bkj));
                reg14 = _mm_add_pd(reg14, _mm_mul_pd(_mm_load_pd(A + n*k + 28 + i), bkj));
                reg15 = _mm_add_pd(reg15, _mm_mul_pd(_mm_load_pd(A + n*k + 30 + i), bkj));
                reg16 = _mm_add_pd(reg16, _mm_mul_pd(_mm_load_pd(A + n*k + 32 + i), bkj));
                reg17 = _mm_add_pd(reg17, _mm_mul_pd(_mm_load_pd(A + n*k + 34 + i), bkj));
                reg18 = _mm_add_pd(reg18, _mm_mul_pd(_mm_load_pd(A + n*k + 36 + i), bkj));
                reg19 = _mm_add_pd(reg19, _mm_mul_pd(_mm_load_pd(A + n*k + 38 + i), bkj));
                reg20 = _mm_add_pd(reg20, _mm_mul_pd(_mm_load_pd(A + n*k + 40 + i), bkj));
                reg21 = _mm_add_pd(reg21, _mm_mul_pd(_mm_load_pd(A + n*k + 42 + i), bkj));
                reg22 = _mm_add_pd(reg22, _mm_mul_pd(_mm_load_pd(A + n*k + 44 + i), bkj));
                reg23 = _mm_add_pd(reg23, _mm_mul_pd(_mm_load_pd(A + n*k + 46 + i), bkj));
                reg24 = _mm_add_pd(reg24, _mm_mul_pd(_mm_load_pd(A + n*k + 48 + i), bkj));
                reg25 = _mm_add_pd(reg25, _mm_mul_pd(_mm_load_pd(A + n*k + 50 + i), bkj));
                reg26 = _mm_add_pd(reg26, _mm_mul_pd(_mm_load_pd(A + n*k + 52 + i), bkj));
                reg27 = _mm_add_pd(reg27, _mm_mul_pd(_mm_load_pd(A + n*k + 54 + i), bkj));
                reg28 = _mm_add_pd(reg28, _mm_mul_pd(_mm_load_pd(A + n*k + 56 + i), bkj));
                reg29 = _mm_add_pd(reg29, _mm_mul_pd(_mm_load_pd(A + n*k + 58 + i), bkj));
                reg30 = _mm_add_pd(reg30, _mm_mul_pd(_mm_load_pd(A + n*k + 60 + i), bkj));
                reg31 = _mm_add_pd(reg31, _mm_mul_pd(_mm_load_pd(A + n*k + 62 + i), bkj));
            }

            _mm_store_pd(C + i + 0 + j*n, reg0);
            _mm_store_pd(C + i + 2 + j*n, reg1);
            _mm_store_pd(C + i + 4 + j*n, reg2);
            _mm_store_pd(C + i + 6 + j*n, reg3);
            _mm_store_pd(C + i + 8 + j*n, reg4);
            _mm_store_pd(C + i + 10 + j*n, reg5);
            _mm_store_pd(C + i + 12 + j*n, reg6);
            _mm_store_pd(C + i + 14 + j*n, reg7);
            _mm_store_pd(C + i + 16 + j*n, reg8);
            _mm_store_pd(C + i + 18 + j*n, reg9);
            _mm_store_pd(C + i + 20 + j*n, reg10);
            _mm_store_pd(C + i + 22 + j*n, reg11);
            _mm_store_pd(C + i + 24 + j*n, reg12);
            _mm_store_pd(C + i + 26 + j*n, reg13);
            _mm_store_pd(C + i + 28 + j*n, reg14);
            _mm_store_pd(C + i + 30 + j*n, reg15);
            _mm_store_pd(C + i + 32 + j*n, reg16);
            _mm_store_pd(C + i + 34 + j*n, reg17);
            _mm_store_pd(C + i + 36 + j*n, reg18);
            _mm_store_pd(C + i + 38 + j*n, reg19);
            _mm_store_pd(C + i + 40 + j*n, reg20);
            _mm_store_pd(C + i + 42 + j*n, reg21);
            _mm_store_pd(C + i + 44 + j*n, reg22);
            _mm_store_pd(C + i + 46 + j*n, reg23);
            _mm_store_pd(C + i + 48 + j*n, reg24);
            _mm_store_pd(C + i + 50 + j*n, reg25);
            _mm_store_pd(C + i + 52 + j*n, reg26);
            _mm_store_pd(C + i + 54 + j*n, reg27);
            _mm_store_pd(C + i + 56 + j*n, reg28);
            _mm_store_pd(C + i + 58 + j*n, reg29);
            _mm_store_pd(C + i + 60 + j*n, reg30);
            _mm_store_pd(C + i + 62 + j*n, reg31);
        }
}

// AVX
void dgemm_avx_blocked(const int n, const double* const A, const double* const B, double* const C)
{
    _block_core(n, A, B, C, _block_avx);
}

void _block_avx(const int n, const int const bi, const int bj, const int bk, const double* const A, const double* const B, double* const C)
{
    for (register int i = bi; i < bi + nb; i += 4)
        for (register int j = bj; j < bj + nb; j++)
        {
            __m256d reg = _mm256_load_pd(C + i + j*n);
            for (register int k = bk; k < bk + nb; k++)
                reg = _mm256_add_pd(reg, _mm256_mul_pd(_mm256_load_pd(A + n*k + i), _mm256_broadcast_sd(B + k + j*n)));
            _mm256_store_pd(C + i + j*n, reg);
        }
}

void dgemm_avx(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 4)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg = _mm256_load_pd(C + i + j*n);
            for (register int k = 0; k < n; k++)
                reg = _mm256_add_pd(reg, _mm256_mul_pd(_mm256_load_pd(A + n*k + i), _mm256_broadcast_sd(B + k + j*n)));
            _mm256_store_pd(C + i + j*n, reg);
        }
}

void dgemm_avx_unroll2(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 8)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_add_pd(reg0, _mm256_mul_pd(_mm256_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm256_add_pd(reg1, _mm256_mul_pd(_mm256_load_pd(A + n*k + 4 + i), bkj));
            }

            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
        }
}

void dgemm_avx_unroll4(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 16)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_add_pd(reg0, _mm256_mul_pd(_mm256_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm256_add_pd(reg1, _mm256_mul_pd(_mm256_load_pd(A + n*k + 4 + i), bkj));
                reg2 = _mm256_add_pd(reg2, _mm256_mul_pd(_mm256_load_pd(A + n*k + 8 + i), bkj));
                reg3 = _mm256_add_pd(reg3, _mm256_mul_pd(_mm256_load_pd(A + n*k + 12 + i), bkj));
            }

            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
        }
}

void dgemm_avx_unroll8(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 32)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            __m256d reg4 = _mm256_load_pd(C + i + 16 + j*n);
            __m256d reg5 = _mm256_load_pd(C + i + 20 + j*n);
            __m256d reg6 = _mm256_load_pd(C + i + 24 + j*n);
            __m256d reg7 = _mm256_load_pd(C + i + 28 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_add_pd(reg0, _mm256_mul_pd(_mm256_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm256_add_pd(reg1, _mm256_mul_pd(_mm256_load_pd(A + n*k + 4 + i), bkj));
                reg2 = _mm256_add_pd(reg2, _mm256_mul_pd(_mm256_load_pd(A + n*k + 8 + i), bkj));
                reg3 = _mm256_add_pd(reg3, _mm256_mul_pd(_mm256_load_pd(A + n*k + 12 + i), bkj));
                reg4 = _mm256_add_pd(reg4, _mm256_mul_pd(_mm256_load_pd(A + n*k + 16 + i), bkj));
                reg5 = _mm256_add_pd(reg5, _mm256_mul_pd(_mm256_load_pd(A + n*k + 20 + i), bkj));
                reg6 = _mm256_add_pd(reg6, _mm256_mul_pd(_mm256_load_pd(A + n*k + 24 + i), bkj));
                reg7 = _mm256_add_pd(reg7, _mm256_mul_pd(_mm256_load_pd(A + n*k + 28 + i), bkj));
            }

            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
            _mm256_store_pd(C + i + 16 + j*n, reg4);
            _mm256_store_pd(C + i + 20 + j*n, reg5);
            _mm256_store_pd(C + i + 24 + j*n, reg6);
            _mm256_store_pd(C + i + 28 + j*n, reg7);
        }
}

void dgemm_avx_unroll16(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 64)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            __m256d reg4 = _mm256_load_pd(C + i + 16 + j*n);
            __m256d reg5 = _mm256_load_pd(C + i + 20 + j*n);
            __m256d reg6 = _mm256_load_pd(C + i + 24 + j*n);
            __m256d reg7 = _mm256_load_pd(C + i + 28 + j*n);
            __m256d reg8 = _mm256_load_pd(C + i + 32 + j*n);
            __m256d reg9 = _mm256_load_pd(C + i + 36 + j*n);
            __m256d reg10 = _mm256_load_pd(C + i + 40 + j*n);
            __m256d reg11 = _mm256_load_pd(C + i + 44 + j*n);
            __m256d reg12 = _mm256_load_pd(C + i + 48 + j*n);
            __m256d reg13 = _mm256_load_pd(C + i + 52 + j*n);
            __m256d reg14 = _mm256_load_pd(C + i + 56 + j*n);
            __m256d reg15 = _mm256_load_pd(C + i + 60 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_add_pd(reg0, _mm256_mul_pd(_mm256_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm256_add_pd(reg1, _mm256_mul_pd(_mm256_load_pd(A + n*k + 4 + i), bkj));
                reg2 = _mm256_add_pd(reg2, _mm256_mul_pd(_mm256_load_pd(A + n*k + 8 + i), bkj));
                reg3 = _mm256_add_pd(reg3, _mm256_mul_pd(_mm256_load_pd(A + n*k + 12 + i), bkj));
                reg4 = _mm256_add_pd(reg4, _mm256_mul_pd(_mm256_load_pd(A + n*k + 16 + i), bkj));
                reg5 = _mm256_add_pd(reg5, _mm256_mul_pd(_mm256_load_pd(A + n*k + 20 + i), bkj));
                reg6 = _mm256_add_pd(reg6, _mm256_mul_pd(_mm256_load_pd(A + n*k + 24 + i), bkj));
                reg7 = _mm256_add_pd(reg7, _mm256_mul_pd(_mm256_load_pd(A + n*k + 28 + i), bkj));
                reg8 = _mm256_add_pd(reg8, _mm256_mul_pd(_mm256_load_pd(A + n*k + 32 + i), bkj));
                reg9 = _mm256_add_pd(reg9, _mm256_mul_pd(_mm256_load_pd(A + n*k + 36 + i), bkj));
                reg10 = _mm256_add_pd(reg10, _mm256_mul_pd(_mm256_load_pd(A + n*k + 40 + i), bkj));
                reg11 = _mm256_add_pd(reg11, _mm256_mul_pd(_mm256_load_pd(A + n*k + 44 + i), bkj));
                reg12 = _mm256_add_pd(reg12, _mm256_mul_pd(_mm256_load_pd(A + n*k + 48 + i), bkj));
                reg13 = _mm256_add_pd(reg13, _mm256_mul_pd(_mm256_load_pd(A + n*k + 52 + i), bkj));
                reg14 = _mm256_add_pd(reg14, _mm256_mul_pd(_mm256_load_pd(A + n*k + 56 + i), bkj));
                reg15 = _mm256_add_pd(reg15, _mm256_mul_pd(_mm256_load_pd(A + n*k + 60 + i), bkj));
            }

            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
            _mm256_store_pd(C + i + 16 + j*n, reg4);
            _mm256_store_pd(C + i + 20 + j*n, reg5);
            _mm256_store_pd(C + i + 24 + j*n, reg6);
            _mm256_store_pd(C + i + 28 + j*n, reg7);
            _mm256_store_pd(C + i + 32 + j*n, reg8);
            _mm256_store_pd(C + i + 36 + j*n, reg9);
            _mm256_store_pd(C + i + 40 + j*n, reg10);
            _mm256_store_pd(C + i + 44 + j*n, reg11);
            _mm256_store_pd(C + i + 48 + j*n, reg12);
            _mm256_store_pd(C + i + 52 + j*n, reg13);
            _mm256_store_pd(C + i + 56 + j*n, reg14);
            _mm256_store_pd(C + i + 60 + j*n, reg15);
        }
}

void dgemm_avx_unroll32(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 128)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            __m256d reg4 = _mm256_load_pd(C + i + 16 + j*n);
            __m256d reg5 = _mm256_load_pd(C + i + 20 + j*n);
            __m256d reg6 = _mm256_load_pd(C + i + 24 + j*n);
            __m256d reg7 = _mm256_load_pd(C + i + 28 + j*n);
            __m256d reg8 = _mm256_load_pd(C + i + 32 + j*n);
            __m256d reg9 = _mm256_load_pd(C + i + 36 + j*n);
            __m256d reg10 = _mm256_load_pd(C + i + 40 + j*n);
            __m256d reg11 = _mm256_load_pd(C + i + 44 + j*n);
            __m256d reg12 = _mm256_load_pd(C + i + 48 + j*n);
            __m256d reg13 = _mm256_load_pd(C + i + 52 + j*n);
            __m256d reg14 = _mm256_load_pd(C + i + 56 + j*n);
            __m256d reg15 = _mm256_load_pd(C + i + 60 + j*n);
            __m256d reg16 = _mm256_load_pd(C + i + 64 + j*n);
            __m256d reg17 = _mm256_load_pd(C + i + 68 + j*n);
            __m256d reg18 = _mm256_load_pd(C + i + 72 + j*n);
            __m256d reg19 = _mm256_load_pd(C + i + 76 + j*n);
            __m256d reg20 = _mm256_load_pd(C + i + 80 + j*n);
            __m256d reg21 = _mm256_load_pd(C + i + 84 + j*n);
            __m256d reg22 = _mm256_load_pd(C + i + 88 + j*n);
            __m256d reg23 = _mm256_load_pd(C + i + 92 + j*n);
            __m256d reg24 = _mm256_load_pd(C + i + 96 + j*n);
            __m256d reg25 = _mm256_load_pd(C + i + 100 + j*n);
            __m256d reg26 = _mm256_load_pd(C + i + 104 + j*n);
            __m256d reg27 = _mm256_load_pd(C + i + 108 + j*n);
            __m256d reg28 = _mm256_load_pd(C + i + 112 + j*n);
            __m256d reg29 = _mm256_load_pd(C + i + 116 + j*n);
            __m256d reg30 = _mm256_load_pd(C + i + 120 + j*n);
            __m256d reg31 = _mm256_load_pd(C + i + 124 + j*n);

            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_add_pd(reg0, _mm256_mul_pd(_mm256_load_pd(A + n*k + 0 + i), bkj));
                reg1 = _mm256_add_pd(reg1, _mm256_mul_pd(_mm256_load_pd(A + n*k + 4 + i), bkj));
                reg2 = _mm256_add_pd(reg2, _mm256_mul_pd(_mm256_load_pd(A + n*k + 8 + i), bkj));
                reg3 = _mm256_add_pd(reg3, _mm256_mul_pd(_mm256_load_pd(A + n*k + 12 + i), bkj));
                reg4 = _mm256_add_pd(reg4, _mm256_mul_pd(_mm256_load_pd(A + n*k + 16 + i), bkj));
                reg5 = _mm256_add_pd(reg5, _mm256_mul_pd(_mm256_load_pd(A + n*k + 20 + i), bkj));
                reg6 = _mm256_add_pd(reg6, _mm256_mul_pd(_mm256_load_pd(A + n*k + 24 + i), bkj));
                reg7 = _mm256_add_pd(reg7, _mm256_mul_pd(_mm256_load_pd(A + n*k + 28 + i), bkj));
                reg8 = _mm256_add_pd(reg8, _mm256_mul_pd(_mm256_load_pd(A + n*k + 32 + i), bkj));
                reg9 = _mm256_add_pd(reg9, _mm256_mul_pd(_mm256_load_pd(A + n*k + 36 + i), bkj));
                reg10 = _mm256_add_pd(reg10, _mm256_mul_pd(_mm256_load_pd(A + n*k + 40 + i), bkj));
                reg11 = _mm256_add_pd(reg11, _mm256_mul_pd(_mm256_load_pd(A + n*k + 44 + i), bkj));
                reg12 = _mm256_add_pd(reg12, _mm256_mul_pd(_mm256_load_pd(A + n*k + 48 + i), bkj));
                reg13 = _mm256_add_pd(reg13, _mm256_mul_pd(_mm256_load_pd(A + n*k + 52 + i), bkj));
                reg14 = _mm256_add_pd(reg14, _mm256_mul_pd(_mm256_load_pd(A + n*k + 56 + i), bkj));
                reg15 = _mm256_add_pd(reg15, _mm256_mul_pd(_mm256_load_pd(A + n*k + 60 + i), bkj));
                reg16 = _mm256_add_pd(reg16, _mm256_mul_pd(_mm256_load_pd(A + n*k + 64 + i), bkj));
                reg17 = _mm256_add_pd(reg17, _mm256_mul_pd(_mm256_load_pd(A + n*k + 68 + i), bkj));
                reg18 = _mm256_add_pd(reg18, _mm256_mul_pd(_mm256_load_pd(A + n*k + 72 + i), bkj));
                reg19 = _mm256_add_pd(reg19, _mm256_mul_pd(_mm256_load_pd(A + n*k + 76 + i), bkj));
                reg20 = _mm256_add_pd(reg20, _mm256_mul_pd(_mm256_load_pd(A + n*k + 80 + i), bkj));
                reg21 = _mm256_add_pd(reg21, _mm256_mul_pd(_mm256_load_pd(A + n*k + 84 + i), bkj));
                reg22 = _mm256_add_pd(reg22, _mm256_mul_pd(_mm256_load_pd(A + n*k + 88 + i), bkj));
                reg23 = _mm256_add_pd(reg23, _mm256_mul_pd(_mm256_load_pd(A + n*k + 92 + i), bkj));
                reg24 = _mm256_add_pd(reg24, _mm256_mul_pd(_mm256_load_pd(A + n*k + 96 + i), bkj));
                reg25 = _mm256_add_pd(reg25, _mm256_mul_pd(_mm256_load_pd(A + n*k + 100 + i), bkj));
                reg26 = _mm256_add_pd(reg26, _mm256_mul_pd(_mm256_load_pd(A + n*k + 104 + i), bkj));
                reg27 = _mm256_add_pd(reg27, _mm256_mul_pd(_mm256_load_pd(A + n*k + 108 + i), bkj));
                reg28 = _mm256_add_pd(reg28, _mm256_mul_pd(_mm256_load_pd(A + n*k + 112 + i), bkj));
                reg29 = _mm256_add_pd(reg29, _mm256_mul_pd(_mm256_load_pd(A + n*k + 116 + i), bkj));
                reg30 = _mm256_add_pd(reg30, _mm256_mul_pd(_mm256_load_pd(A + n*k + 120 + i), bkj));
                reg31 = _mm256_add_pd(reg31, _mm256_mul_pd(_mm256_load_pd(A + n*k + 124 + i), bkj));
            }

            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
            _mm256_store_pd(C + i + 16 + j*n, reg4);
            _mm256_store_pd(C + i + 20 + j*n, reg5);
            _mm256_store_pd(C + i + 24 + j*n, reg6);
            _mm256_store_pd(C + i + 28 + j*n, reg7);
            _mm256_store_pd(C + i + 32 + j*n, reg8);
            _mm256_store_pd(C + i + 36 + j*n, reg9);
            _mm256_store_pd(C + i + 40 + j*n, reg10);
            _mm256_store_pd(C + i + 44 + j*n, reg11);
            _mm256_store_pd(C + i + 48 + j*n, reg12);
            _mm256_store_pd(C + i + 52 + j*n, reg13);
            _mm256_store_pd(C + i + 56 + j*n, reg14);
            _mm256_store_pd(C + i + 60 + j*n, reg15);
            _mm256_store_pd(C + i + 64 + j*n, reg16);
            _mm256_store_pd(C + i + 68 + j*n, reg17);
            _mm256_store_pd(C + i + 72 + j*n, reg18);
            _mm256_store_pd(C + i + 76 + j*n, reg19);
            _mm256_store_pd(C + i + 80 + j*n, reg20);
            _mm256_store_pd(C + i + 84 + j*n, reg21);
            _mm256_store_pd(C + i + 88 + j*n, reg22);
            _mm256_store_pd(C + i + 92 + j*n, reg23);
            _mm256_store_pd(C + i + 96 + j*n, reg24);
            _mm256_store_pd(C + i + 100 + j*n, reg25);
            _mm256_store_pd(C + i + 104 + j*n, reg26);
            _mm256_store_pd(C + i + 108 + j*n, reg27);
            _mm256_store_pd(C + i + 112 + j*n, reg28);
            _mm256_store_pd(C + i + 116 + j*n, reg29);
            _mm256_store_pd(C + i + 120 + j*n, reg30);
            _mm256_store_pd(C + i + 124 + j*n, reg31);
        }
}

// FMA
void dgemm_avx_fma_blocked(const int n, const double* const A, const double* const B, double* const C)
{
    _block_core(n, A, B, C, _block_avx_fma);
}

void _block_avx_fma(const int n, const int const bi, const int bj, const int bk, const double* const A, const double* const B, double* const C)
{
    for (register int i = bi; i < bi + nb; i += 4)
        for (register int j = bj; j < bj + nb; j++)
        {
            __m256d reg = _mm256_load_pd(C + i + j*n);
            for (register int k = bk; k < bk + nb; k++)
                reg = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + i), _mm256_broadcast_sd(B + k + j*n), reg);
            _mm256_store_pd(C + i + j*n, reg);
        }
}

void dgemm_avx_fma(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 4)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg = _mm256_load_pd(C + i + 0 + j*n);
            for (register int k = 0; k < n; k++)
                reg = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 0 + i), _mm256_broadcast_sd(B + k + j*n), reg);
            _mm256_store_pd(C + i + 0 + j*n, reg);
        }
}

void dgemm_avx_fma_unroll2(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 8)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 0 + i), bkj, reg0);
                reg1 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 4 + i), bkj, reg1);
            }
            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
        }
}

void dgemm_avx_fma_unroll4(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 16)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 0 + i), bkj, reg0);
                reg1 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 4 + i), bkj, reg1);
                reg2 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 8 + i), bkj, reg2);
                reg3 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 12 + i), bkj, reg3);
            }
            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
        }
}

void dgemm_avx_fma_unroll8(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 32)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            __m256d reg4 = _mm256_load_pd(C + i + 16 + j*n);
            __m256d reg5 = _mm256_load_pd(C + i + 20 + j*n);
            __m256d reg6 = _mm256_load_pd(C + i + 24 + j*n);
            __m256d reg7 = _mm256_load_pd(C + i + 28 + j*n);
            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 0 + i), bkj, reg0);
                reg1 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 4 + i), bkj, reg1);
                reg2 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 8 + i), bkj, reg2);
                reg3 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 12 + i), bkj, reg3);
                reg4 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 16 + i), bkj, reg4);
                reg5 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 20 + i), bkj, reg5);
                reg6 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 24 + i), bkj, reg6);
                reg7 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 28 + i), bkj, reg7);
            }
            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
            _mm256_store_pd(C + i + 16 + j*n, reg4);
            _mm256_store_pd(C + i + 20 + j*n, reg5);
            _mm256_store_pd(C + i + 24 + j*n, reg6);
            _mm256_store_pd(C + i + 28 + j*n, reg7);
        }
}

void dgemm_avx_fma_unroll16(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 64)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            __m256d reg4 = _mm256_load_pd(C + i + 16 + j*n);
            __m256d reg5 = _mm256_load_pd(C + i + 20 + j*n);
            __m256d reg6 = _mm256_load_pd(C + i + 24 + j*n);
            __m256d reg7 = _mm256_load_pd(C + i + 28 + j*n);
            __m256d reg8 = _mm256_load_pd(C + i + 32 + j*n);
            __m256d reg9 = _mm256_load_pd(C + i + 36 + j*n);
            __m256d reg10 = _mm256_load_pd(C + i + 40 + j*n);
            __m256d reg11 = _mm256_load_pd(C + i + 44 + j*n);
            __m256d reg12 = _mm256_load_pd(C + i + 48 + j*n);
            __m256d reg13 = _mm256_load_pd(C + i + 52 + j*n);
            __m256d reg14 = _mm256_load_pd(C + i + 56 + j*n);
            __m256d reg15 = _mm256_load_pd(C + i + 60 + j*n);
            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 0 + i), bkj, reg0);
                reg1 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 4 + i), bkj, reg1);
                reg2 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 8 + i), bkj, reg2);
                reg3 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 12 + i), bkj, reg3);
                reg4 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 16 + i), bkj, reg4);
                reg5 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 20 + i), bkj, reg5);
                reg6 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 24 + i), bkj, reg6);
                reg7 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 28 + i), bkj, reg7);
                reg8 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 32 + i), bkj, reg8);
                reg9 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 36 + i), bkj, reg9);
                reg10 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 40 + i), bkj, reg10);
                reg11 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 44 + i), bkj, reg11);
                reg12 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 48 + i), bkj, reg12);
                reg13 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 52 + i), bkj, reg13);
                reg14 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 56 + i), bkj, reg14);
                reg15 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 60 + i), bkj, reg15);
            }
            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
            _mm256_store_pd(C + i + 16 + j*n, reg4);
            _mm256_store_pd(C + i + 20 + j*n, reg5);
            _mm256_store_pd(C + i + 24 + j*n, reg6);
            _mm256_store_pd(C + i + 28 + j*n, reg7);
            _mm256_store_pd(C + i + 32 + j*n, reg8);
            _mm256_store_pd(C + i + 36 + j*n, reg9);
            _mm256_store_pd(C + i + 40 + j*n, reg10);
            _mm256_store_pd(C + i + 44 + j*n, reg11);
            _mm256_store_pd(C + i + 48 + j*n, reg12);
            _mm256_store_pd(C + i + 52 + j*n, reg13);
            _mm256_store_pd(C + i + 56 + j*n, reg14);
            _mm256_store_pd(C + i + 60 + j*n, reg15);
        }
}

void dgemm_avx_fma_unroll32(const int n, const double* const A, const double* const B, double* const C)
{
    for (register int i = 0; i < n; i += 128)
        for (register int j = 0; j < n; j++)
        {
            __m256d reg0 = _mm256_load_pd(C + i + 0 + j*n);
            __m256d reg1 = _mm256_load_pd(C + i + 4 + j*n);
            __m256d reg2 = _mm256_load_pd(C + i + 8 + j*n);
            __m256d reg3 = _mm256_load_pd(C + i + 12 + j*n);
            __m256d reg4 = _mm256_load_pd(C + i + 16 + j*n);
            __m256d reg5 = _mm256_load_pd(C + i + 20 + j*n);
            __m256d reg6 = _mm256_load_pd(C + i + 24 + j*n);
            __m256d reg7 = _mm256_load_pd(C + i + 28 + j*n);
            __m256d reg8 = _mm256_load_pd(C + i + 32 + j*n);
            __m256d reg9 = _mm256_load_pd(C + i + 36 + j*n);
            __m256d reg10 = _mm256_load_pd(C + i + 40 + j*n);
            __m256d reg11 = _mm256_load_pd(C + i + 44 + j*n);
            __m256d reg12 = _mm256_load_pd(C + i + 48 + j*n);
            __m256d reg13 = _mm256_load_pd(C + i + 52 + j*n);
            __m256d reg14 = _mm256_load_pd(C + i + 56 + j*n);
            __m256d reg15 = _mm256_load_pd(C + i + 60 + j*n);
            __m256d reg16 = _mm256_load_pd(C + i + 64 + j*n);
            __m256d reg17 = _mm256_load_pd(C + i + 68 + j*n);
            __m256d reg18 = _mm256_load_pd(C + i + 72 + j*n);
            __m256d reg19 = _mm256_load_pd(C + i + 76 + j*n);
            __m256d reg20 = _mm256_load_pd(C + i + 80 + j*n);
            __m256d reg21 = _mm256_load_pd(C + i + 84 + j*n);
            __m256d reg22 = _mm256_load_pd(C + i + 88 + j*n);
            __m256d reg23 = _mm256_load_pd(C + i + 92 + j*n);
            __m256d reg24 = _mm256_load_pd(C + i + 96 + j*n);
            __m256d reg25 = _mm256_load_pd(C + i + 100 + j*n);
            __m256d reg26 = _mm256_load_pd(C + i + 104 + j*n);
            __m256d reg27 = _mm256_load_pd(C + i + 108 + j*n);
            __m256d reg28 = _mm256_load_pd(C + i + 112 + j*n);
            __m256d reg29 = _mm256_load_pd(C + i + 116 + j*n);
            __m256d reg30 = _mm256_load_pd(C + i + 120 + j*n);
            __m256d reg31 = _mm256_load_pd(C + i + 124 + j*n);
            for (register int k = 0; k < n; k++)
            {
                __m256d bkj = _mm256_broadcast_sd(B + k + j*n);
                reg0 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 0 + i), bkj, reg0);
                reg1 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 4 + i), bkj, reg1);
                reg2 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 8 + i), bkj, reg2);
                reg3 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 12 + i), bkj, reg3);
                reg4 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 16 + i), bkj, reg4);
                reg5 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 20 + i), bkj, reg5);
                reg6 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 24 + i), bkj, reg6);
                reg7 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 28 + i), bkj, reg7);
                reg8 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 32 + i), bkj, reg8);
                reg9 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 36 + i), bkj, reg9);
                reg10 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 40 + i), bkj, reg10);
                reg11 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 44 + i), bkj, reg11);
                reg12 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 48 + i), bkj, reg12);
                reg13 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 52 + i), bkj, reg13);
                reg14 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 56 + i), bkj, reg14);
                reg15 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 60 + i), bkj, reg15);
                reg16 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 64 + i), bkj, reg16);
                reg17 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 68 + i), bkj, reg17);
                reg18 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 72 + i), bkj, reg18);
                reg19 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 76 + i), bkj, reg19);
                reg20 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 80 + i), bkj, reg20);
                reg21 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 84 + i), bkj, reg21);
                reg22 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 88 + i), bkj, reg22);
                reg23 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 92 + i), bkj, reg23);
                reg24 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 96 + i), bkj, reg24);
                reg25 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 100 + i), bkj, reg25);
                reg26 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 104 + i), bkj, reg26);
                reg27 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 108 + i), bkj, reg27);
                reg28 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 112 + i), bkj, reg28);
                reg29 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 116 + i), bkj, reg29);
                reg30 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 120 + i), bkj, reg30);
                reg31 = _mm256_fmadd_pd(_mm256_load_pd(A + n*k + 124 + i), bkj, reg31);
            }
            _mm256_store_pd(C + i + 0 + j*n, reg0);
            _mm256_store_pd(C + i + 4 + j*n, reg1);
            _mm256_store_pd(C + i + 8 + j*n, reg2);
            _mm256_store_pd(C + i + 12 + j*n, reg3);
            _mm256_store_pd(C + i + 16 + j*n, reg4);
            _mm256_store_pd(C + i + 20 + j*n, reg5);
            _mm256_store_pd(C + i + 24 + j*n, reg6);
            _mm256_store_pd(C + i + 28 + j*n, reg7);
            _mm256_store_pd(C + i + 32 + j*n, reg8);
            _mm256_store_pd(C + i + 36 + j*n, reg9);
            _mm256_store_pd(C + i + 40 + j*n, reg10);
            _mm256_store_pd(C + i + 44 + j*n, reg11);
            _mm256_store_pd(C + i + 48 + j*n, reg12);
            _mm256_store_pd(C + i + 52 + j*n, reg13);
            _mm256_store_pd(C + i + 56 + j*n, reg14);
            _mm256_store_pd(C + i + 60 + j*n, reg15);
            _mm256_store_pd(C + i + 64 + j*n, reg16);
            _mm256_store_pd(C + i + 68 + j*n, reg17);
            _mm256_store_pd(C + i + 72 + j*n, reg18);
            _mm256_store_pd(C + i + 76 + j*n, reg19);
            _mm256_store_pd(C + i + 80 + j*n, reg20);
            _mm256_store_pd(C + i + 84 + j*n, reg21);
            _mm256_store_pd(C + i + 88 + j*n, reg22);
            _mm256_store_pd(C + i + 92 + j*n, reg23);
            _mm256_store_pd(C + i + 96 + j*n, reg24);
            _mm256_store_pd(C + i + 100 + j*n, reg25);
            _mm256_store_pd(C + i + 104 + j*n, reg26);
            _mm256_store_pd(C + i + 108 + j*n, reg27);
            _mm256_store_pd(C + i + 112 + j*n, reg28);
            _mm256_store_pd(C + i + 116 + j*n, reg29);
            _mm256_store_pd(C + i + 120 + j*n, reg30);
            _mm256_store_pd(C + i + 124 + j*n, reg31);
        }
}
