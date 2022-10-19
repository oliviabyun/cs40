/* readaline.c
 *
 * by Helena Lowe (hlowe01) and Olivia Byun (obyun01)
 * 9/20/22
 * HW1: filesofpix
 *
 * SUMMARY: This file reads a line from a given file and adds each character.
 *          in the line to a given char array.
 *          It is used with the restoration.c program.
 * 
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "readaline.h"
#include <assert.h>

extern size_t readaline(FILE *inputfd, char **datapp);

/*
* name:      readaline
* purpose:   read a single line of input from a given input file
* arguments: pointer to the first character in the file, a pointer to a char
             array storing a line
* returns:   the number of bytes in the line
* other:     terminates with a checked runtime error if either or both
             inputs are null
*/
extern size_t readaline(FILE *inputfd, char **datapp) {
        assert (inputfd != NULL); /* terminate with CRE if null */
        assert (datapp != NULL);

        /* malloc datapp and confirm memory allocation */
        int mem_size = 1000;
        *datapp = (char*) malloc(mem_size * sizeof(char));
        assert (datapp != NULL);

        char c = fgetc(inputfd); /* get first character in line */
        int i = 0;

        while (c != EOF && c != '\n') { /* loop until end of line */
                if (i > (mem_size - 10)) {
                        /* reallocate memory if line is too long */
                        mem_size = mem_size * 1.5;
                        *datapp = (char*) realloc(*datapp, 
                                  (mem_size * sizeof(char)));
                }

                (*datapp)[i] = c; /* add character to array */
                i += 1;
                c = fgetc(inputfd); /* get next character */
        }

        (*datapp)[i] = '\0'; /* update null-terminating characters */

        return i; /* return # of bytes in the line */
}