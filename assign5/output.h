#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdbool.h>

#define MIN(x, y) (x) < (y) ? (x) : (y)

bool writebytes(unsigned long long x, int nbytes);
void generate_random_word(unsigned long long x, char *word);
void write_chunks(char *buf, int nbytes, int N);
#endif