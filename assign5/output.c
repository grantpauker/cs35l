#include "output.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

bool writebytes(unsigned long long x, int nbytes)
{
  do
  {
    if (putchar(x) < 0)
      return false;
    x >>= CHAR_BIT;
    nbytes--;
  } while (0 < nbytes);

  return true;
}

void generate_random_word(unsigned long long x, char *word)
{
  for (int i = 0; i < 8; i++)
  {
    word[i] = (char)x;
    x >>= 8;
  }
}

void write_chunks(char *buf, int nbytes, int N)
{
  for (int i = 0; i < nbytes; i += N)
  {
    write(1, buf + i, MIN(N, nbytes - i));
  }
}