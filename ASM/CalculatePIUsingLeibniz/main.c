#include <stdio.h>

#define N 100000000000
#define MY_PI  3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628

long double leibniz(unsigned long long x);
int main()
{
    for (unsigned long long i = 10; i < N; i *= 10)
    {
        long double result = leibniz((unsigned long long)i) * (long double)4;
        printf("Results for: %llu\n", (unsigned long long)i);
        printf("Result: %1.30Lf\n", result);
        printf("Difference from math.PI: %1.30Lf\n", result - MY_PI);
    }
    return 0;
}