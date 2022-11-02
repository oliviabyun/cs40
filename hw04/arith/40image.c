/*
 *     40image.c
 *     Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This file takes in command line arguments to either compress or
 *     decompress a given image file.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

/* 
 *      name: main
 *   purpose: takes in command line arguments to either compress or
 *            decompress a given image file.
 *    inputs: argc - number of command line arguments (integer)
 *            argv - the command line arguments (char array)
 *   outputs: EXIT_SUCCESS if program is successful; EXIT_FAILURE otherwise
 *    errors: throws a CRE if there is more than one file on the command line
 */
int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}
