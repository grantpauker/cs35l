#include "rand64-sw.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *urandstream;
bool mrandr48;
struct drand48_data buf;

/* Initialize the software rand64 implementation.  */
void software_rand64_init(char *filename, bool _mrandr48)
{
  mrandr48 = _mrandr48;
  if (mrandr48)
  {
    srand48_r(time(NULL), &buf);
  }
  else
  {
    if (filename[0] == '\0')
    {
      strcpy(filename, "/dev/random");
    }
    urandstream = fopen(filename, "r");
    if (!urandstream)
    {
      abort();
    }
  }
}

/* Return a random value, using software operations.  */
unsigned long long software_rand64(void)
{
  unsigned long long int x;
  if (mrandr48)
  {
    long upper;
    long lower;
    mrand48_r(&buf, &lower);
    mrand48_r(&buf, &upper);
    x = (upper << 32) | lower;
  }
  else
  {
    if (fread(&x, sizeof x, 1, urandstream) != 1)
    {
      abort();
    }
  }
  return x;
}

/* Finalize the software rand64 implementation.  */
void software_rand64_fini(void)
{
  if (!mrandr48)
  {
    fclose(urandstream);
  }
}