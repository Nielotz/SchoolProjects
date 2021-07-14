as ak.s -o ak.o && gcc --shared ak.o -o dyn_lib.so && gcc -c main.c && gcc main.o dyn_lib.so -o run && rm *.o && LD_LIBRARY_PATH=. ./run "$@"
