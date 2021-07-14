#include <stdlib.h>

unsigned int GCD(unsigned long long, unsigned long long);
void print_call_rsp();
void print_ret_rsp();

int main(int args, char* argv[])
{
    //print_call_rsp();
    GCD(atoll(argv[1]), atoll(argv[2]));
    //print_ret_rsp();
}