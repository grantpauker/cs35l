/* Generate N bytes of random output. */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable. Compile it with gcc -mrdrnd for a
   x86-64 machine.

   Copyright 2015, 2017, 2020 Paul Eggert

   This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include "options.h"
#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"
#include <cpuid.h>
#include <errno.h>
#include <immintrin.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN(x, y) (x) < (y) ? (x) : (y)

/* Main program, which outputs N bytes of random data. */
int main(int argc, char **argv)
{

  long long nbytes;
  bool rdrand = false;
  bool mrand48 = false;
  char filename[4096];
  bool stdio = false;
  unsigned int N;
  bool chunks = false;

  int status = get_options(argc, argv, &nbytes, &rdrand, &mrand48, filename, &N, &chunks);

  if (status)
  {
    return status;
  }

  /* If there's no work to do, don't worry about which library to use. */
  if (nbytes == 0)
  {
    return 0;
  }

  /* Now that we know we have work to do, arrange to use the
     appropriate library. */
  unsigned long long (*rand64)(void);
  void (*finalize)(void);
  if (rdrand)
  {
    if (rdrand_supported())
    {
      hardware_rand64_init();
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
    }
    else
    {
      fprintf(stderr, "error: rdrand not supported\n");
      return 1;
    }
  }
  else
  {
    software_rand64_init(filename, mrand48);
    rand64 = software_rand64;
    finalize = software_rand64_fini;
  }

  int output_errno = 0;
  if (!chunks)
  {
    int wordsize = sizeof rand64();

    do
    {
      unsigned long long x = rand64();
      int outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (!writebytes(x, outbytes))
      {
        output_errno = errno;
        break;
      }
      nbytes -= outbytes;
    } while (0 < nbytes);

    if (fclose(stdout) != 0)
    {
      output_errno = errno;
    }

    if (output_errno)
    {
      errno = output_errno;
      perror("output");
    }
  }
  else
  {
    char *buf = (char *)malloc(nbytes);
    unsigned long long x;
    char rand_word[8];
    for (int i = 0; i < nbytes; i += 8)
    {
      x = rand64();
      generate_random_word(x, rand_word);
      memcpy(buf + i, rand_word, MIN(8, nbytes - i));
    }

    write_chunks(buf, nbytes, N);

    free(buf);
  }

  finalize();
  return !!output_errno;
}
