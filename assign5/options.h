#ifndef OPTIONS_H
#define OPTIONS_H
#include <stdbool.h>
int get_options(int argc, char **argv, long long *nbytes, bool *randr, bool *mrand48, char **file, unsigned int *N, bool *chunks);
#endif