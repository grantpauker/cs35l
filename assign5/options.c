#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int get_options(int argc, char **argv, long long *nbytes, bool *rdrand, bool *mrand48, char *filename, unsigned int *N, bool *chunks)
{
    int c;
    while ((c = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (c)
        {
        case 'i':
            if (strcmp(optarg, "rdrand") == 0)
            {
                *rdrand = true;
            }
            else if (strcmp(optarg, "mrand48_r") == 0)
            {
                *mrand48 = true;
                strcpy(filename, "");
            }
            else if (optarg[0] == '/')
            {
                strcpy(filename, optarg);
            }
            else
            {
                fprintf(stderr, "error: -i option requires one of: rdrand|mrand48_r|/path\n");
                return 1;
            }
            break;
        case 'o':
            if (strcmp(optarg, "stdio") == 0)
            {
                break;
            }
            else
            {
                *chunks = true;
                char *endptr;
                bool valid;
                errno = 0;
                *N = strtoull(optarg, &endptr, 10);
                if (errno)
                {
                    perror(optarg);
                    return 1;
                }
                else
                {
                    valid = !*endptr && 0 <= *nbytes;
                }
                if (!valid)
                {
                    fprintf(stderr, "error: N not valid\n");
                    return 1;
                }
            }
            break;
        }
    }
    if (!(*mrand48) && !(*rdrand) && (filename[0] != '/'))
    {
        *rdrand = true;
    }
    if (argc > optind)
    {
        bool valid;
        char *endptr;
        errno = 0;
        *nbytes = strtoll(argv[optind], &endptr, 10);
        if (errno)
        {
            perror(argv[optind]);
            return 1;
        }
        else
        {
            valid = !*endptr && 0 <= *nbytes;
        }
        if (!valid)
        {
            fprintf(stderr, "error: NBYTES not valid\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "error: NBYTES not specified\n");
        return 1;
    }
    return 0;
}