#ifndef RAND64_SW_H
#define RAND64_SW_H
#include <stdio.h>
#include <stdbool.h>

void software_rand64_init(char *filename, bool _mrand48);

unsigned long long software_rand64(void);

void software_rand64_fini(void);
#endif