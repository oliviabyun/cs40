/* restoration.c
 *
 * by Helena Lowe (hlowe01) and Olivia Byun (obyun01)
 * 9/20/22
 * HW1: filesofpix
 *
 * SUMMARY: This file uses command line arguments to get a corrupted plain pgm
 *      file and uses readaline.c to read each line in the corrupted file. 
 *      Each line is separated into digit and nondigit arrays. The original 
 *      plain formatted pixel values are converted to raw-formatted characters
 *      and the uncorrupted raw pgm is printed out to stdout.
 * 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include "readaline.h"
#include <ctype.h>
#include "atom.h"
#include "table.h"
#include <string.h>
#include <assert.h>

#define T Table_T

/* NOTE: COMMENTED OUT VARIABLES WERE USED FOR TESTING PURPOSES */
static FILE *open_file(char *filename, char *mode);
static char digit_to_char(char *digits);
static int process_line(char *datapp, unsigned length,
                        Table_T table, char **main_key, 
                        char *original_pixels, int *original_i, int *height);
static int string_to_int(char *digits);
static void free_all(FILE *fp, char *main_key, Table_T *table, 
                     char *original_pixels/*, FILE *raw*/);
static void process_digit(char *datapp, int *digits_i, char **digits, int i);
static void process_nondigit(char **digits, int *digits_i, char **pixels, 
                          int *pixels_i, char **injected, int *injected_i,
                          char *datapp, int i, int *w);
static int concatenate_original(char **str, int original_i,
                                char **original_pixels, int pixels_i);
static void vfree(const void *key, void **value, void *c1);

/*
* name:      main
* purpose:   restore a corrupted plain image and convert to raw
* arguments: number of command line arguments and the command line arguments
* returns:   EXIT_SUCCESS
* effects:   corrupted plain image is restored and converted to raw image
*            raw image is printed to stdout.
*/
int main(int argc, char *argv[]) {
        assert (argc == 2); /* make sure # of arguments is correct */

        /* open specified files */
        FILE *fp = open_file(argv[1], "r");
        char *datapp = NULL;
        
        unsigned length = readaline(fp, &datapp); /* get length */

        /* initialize height, width, and index variables*/
        int height = 0, width = 0, original_i = 0;

        /* malloc main key and confirm memory allocation */
        char *main_key = (char*) malloc(length * sizeof(char));
        assert (main_key != NULL);
        main_key[0] = '\0';

        Table_T table = Table_new(0, NULL, NULL); /* initialize new Table */

        /* malloc original pixels and confirm memory allocation */
        int mem_size = 7000;
        char *original_pixels = (char*) malloc((mem_size) * sizeof(char));
        assert (original_pixels != NULL);
        original_pixels[0] = '\0';

        /* process one line */
        width = process_line(datapp, length, table, &main_key, original_pixels,
                             &original_i, &height);
        free(datapp);

        /* loop until reaching end of given file */
        while (!feof(fp)) {
                length = readaline(fp, &datapp); /* read in one line */

                /* reallocate memory if necessary */
                if ((width * height) > (mem_size - 5000)) {
                        mem_size = mem_size * 1.5;
                        original_pixels = (char*) realloc(original_pixels, 
                                          mem_size);
                }

                if (length != 0) {
                        /* process one line */
                        int w = process_line(datapp, length, table, &main_key,
                                             original_pixels, &original_i,
                                             &height);
                        if (w != -1) {
                                width = w; /* update width */
                        }
                }
                free(datapp); /* free heap-allocated memory */
        }

        /* open file and output raw header */

        /* NOTE: COMMENTED OUT VARIABLES USED FOR TESTING PURPOSES:
           used to test the program. displayed images after running.
        
        FILE *raw = fopen("raw.pgm", "w");
        fprintf(raw, "%s\n%u %u\n%u\n", "P5", width, height, 255);
        */
        fprintf(stdout, "%s\n%u %u\n%u\n", "P5", width, height, 255);

        /* output each pixel to file */
        for (int i = 0; i < original_i; i++) {
                /* fprintf(raw, "%c", original_pixels[i]); NOTE:used to test. */
                fprintf(stdout, "%c", original_pixels[i]);
        }
        
        /* free all remaining heap-allocated memory. NOTE: raw used to test */
        free_all(fp, main_key, &table, original_pixels/*, raw*/);
        return EXIT_SUCCESS;
}

/*
* name:      free_all
* purpose:   free all memory allocated in main.
* arguments: pointer to the image file, the char array containing the 
             main_key (injected string), the table, and the original pixels 
             char array. NOTE: raw was used for testing purposes
* returns:   none
* effects:   fp, main_key, table, and original_pixels are freed.
*/
static void free_all(FILE *fp, char *main_key, Table_T *table, 
                char *original_pixels/*, FILE *raw*/) {
        fclose(fp);
        /* fclose(raw); NOTE: used for testing purposes */
        free(main_key);
        Table_map(*table, vfree, NULL);
        Table_free(table);
        free(original_pixels);
}

/*
* name:      vfree
* purpose:   free the values in table
* arguments: the key, the value, pointer that is applied at each call
* returns:   none
* effects:   frees key and value in table
*/
static void vfree(const void *key, void **value, void *c1) {
        free(*value);
        (void)key;
        (void)c1;
}

/*
* name:      process_line
* purpose:   process a line from the file by separating nondigits from digits.
* arguments: the line from readaline.
		The length of the line from readaline.
		The table to store key, value pairs.
		The main_key injected string.
		The large original pixels char array.
		The current index in the original pixels char array.
		The current height of the image to be incremented if an original
                
                line is found.
* returns:   the width of the line if it is a line of original pixels.
                 -1 
             otherwise.
* effects:   original_pixels updated with new pixel values if line had original
             pixels, original_i updated with new index if line had original 
             pixels, table updated with new atom key and pixel value if main 
             key not found
*/
static int process_line(char *datapp, unsigned length, Table_T table, 
                        char **main_key, char *original_pixels, int *original_i,
                        int *height) {
        /* malloc array to store unconverted numbers, confirm malloc */
        char *digits = (char*) malloc(4 * sizeof(char)); 
        assert (digits != NULL);

        /* malloc array to store injected string, confirm malloc */
        char *injected = (char*) malloc((length + 1) * sizeof(char));
        assert (injected != NULL);

        /* malloc array to store converted chars, confirm malloc */
        char *pixels = (char*) malloc(length * sizeof(char));
        assert (pixels != NULL);

        /* initialize indices and char arrays */
        int digits_i = 0, injected_i = 0, pixels_i = 0, width = 0;
        digits[digits_i] = '\0';
        injected[injected_i] = '\0';
        pixels[pixels_i] = '\0';

        /* loop through each char in the line */
        for (unsigned i = 0; i < length; i++) {
                if (!(isdigit(datapp[i]))) { /* not a digit */
                        /* process nondigit character */
                        process_nondigit(&digits, &digits_i, &pixels, &pixels_i,
                                &injected, &injected_i, datapp, i, 
                                         &width);
                } else { /* is a digit */
                        /* process digit character */
                        process_digit(datapp, &digits_i, &digits, i);
                        if (i == (length - 1)) {
                                /* convert digits left at end of row */
                                digits_i = 0;
                                pixels[pixels_i] = digit_to_char(digits);
                                width++;
                                digits[digits_i] = '\0';
                                pixels_i++;
                                pixels[pixels_i] = '\0';
                        }
                }
        }

        /* convert injected string to Atom, see if it exists in table */
        const char *atom = Atom_string(injected);
        char *str = Table_get(table, atom);        

        if (strlen(*main_key) != 0) { /* main key already exists */
                /* add pixels to original_pixels if atom matches main key */
                if (strcmp(injected, *main_key) == 0) {
                        *original_i = concatenate_original(&pixels, *original_i,
                                &original_pixels, pixels_i);
                        *height = *height + 1; /* increment height */
                } else {
                        width = -1;
                }
                free(pixels);
        } else if (str != NULL) { /* found duplicate key: set main key */
                *main_key = (char*) realloc(*main_key, length + 1);
                
                /* copy each character of injected to main_key */
                for (int i = 0; i < injected_i; i++) {
                        (*main_key)[i] = injected[i];
                }
                (*main_key)[injected_i] = '\0'; /* update null character */

                /* put str then pixels into original_pixels */
                *original_i = concatenate_original(&str, *original_i, 
                                &original_pixels, pixels_i);
                *original_i = concatenate_original(&pixels, *original_i, 
                                &original_pixels, pixels_i);
                
                /* increment height by 2 since we're adding 2 rows */
                *height = *height + 2;
                free(pixels);
        } else { /*key doesn't exist in the Table and main_key doesn't exist */
                Table_put(table, atom, pixels); /* add pixels to Table */
                width = -1;
        }
        /* free heap-allocated memory */
        free(digits);
        free(injected);
        return width; /* return width of original row*/
}

/*
* name:      concatenate_original
* purpose:   add original pixels from current line to larger original pixels 
             char array
* arguments: the char array containing the original pixels in the current line
		The index of the large original_pixels char array.
		The large original_pixels char array.
		The index of the current line’s pixels char array.
* returns:   the index of the original pixels char array after adding the 
             current line’s pixels.
* effects:   the original pixels from the current line are added to the larger 
             original pixels char array.
*/
static int concatenate_original(char **pixels, int original_i,
                                char **original_pixels, int pixels_i) {
        /* loop through each character in pixels */
        for (int i = 0; i < pixels_i; i++) {
                /* copy pixels character into curr index of original pixels */
                (*original_pixels)[original_i] = (*pixels)[i];
                
                if (i == (pixels_i - 1)) { /* set null char if at the end */
                        (*original_pixels)[original_i + 1] = '\0';
                }
                original_i++; /* increment original_pixels index */
        }

        return original_i;
}

/* 
 * name:      process_nondigit
 * purpose:   process nondigit character by adding to our injected char array.
 * arguments: digits, pixels, and injected char arrays.
              digits_i, pixels_i, and injected_i index values.
              width of the line.
              datapp (the character line from readaline).
              i (the index in the line -- datapp).
 * returns:   none
 * effects:   nondigit character added to injected char array.
              digits char array is converted to raw character format and added
              to pixels char array.
 */
static void process_nondigit(char **digits, int *digits_i, char **pixels, 
                          int *pixels_i, char **injected, int *injected_i,
                          char *datapp, int i, int *width) {
        /* convert digits to integers, if existent */
        if ((*digits)[0] != '\0') {
                *digits_i = 0; /* reset digits index */
                (*pixels)[*pixels_i] = digit_to_char(*digits);
                *width = *width + 1; /* increment width */
                *pixels_i = *pixels_i + 1; /* increment pixels index */
                (*pixels)[*pixels_i] = '\0'; /* update null character */

                /* reset digits by reallocing memory */
                *digits = (char*) realloc(*digits, (4 * sizeof(char))); /*  */
                assert (digits != NULL);
                (*digits)[*digits_i] = '\0';
        }

        /* add character to the injected string */
        (*injected)[*injected_i] = datapp[i];
        *injected_i = *injected_i + 1;
        (*injected)[*injected_i] = '\0';
}

/*
* name:      process_digit
* purpose:   add the current digit character to the digits char array, to be 
             converted to an integer and then converted to raw formatted 
             character when the end of the integer is detected.
* arguments: the current line, the current index of the digits char array, the 
             digits char array, the current index in the line.
* returns:   none
* effects:   the current digit character is added to the digits char array.
*/
static void process_digit(char *datapp, int *digits_i, char **digits, int i) {
        /* store digit in appropriate slot in digits array */
        (*digits)[*digits_i] = datapp[i];

        *digits_i = *digits_i + 1; /* increment index of digits */
        (*digits)[*digits_i] = '\0'; /* update null character */
}

/*
* name:      digit_to_char
* purpose:   convert the digits in the char array into an integer 
             and then convert that integer into the raw-formatted character.
* arguments: the digits char array
* returns:   the converted character representation of the given digits
* effects:   none
*/
static char digit_to_char(char *digits) {
        /* convert digits (string array) to a int */
        int digits_int = string_to_int(digits);

        /* cast integer to character */
        char character = digits_int;

        return character; /* return converted character */
}

/*
* name:      string_to_int
* purpose:   convert the digits char array into its integer form.
* arguments: the digits char array.
* returns:   the integer form of the digits char array.
* effects:   none
*/
static int string_to_int(char *digits) {
        int n = 0;

        /* loop through each character in digits */
        for (size_t i = 0; i < strlen(digits); i++) { 
                /* add new digit to n; obtain integer value */
                n = (n * 10) + (digits[i] - 48);
        }

        return n; /* return converted integer */
}

/*
* name:      open_file
* purpose:   opens the given file in the given mode.
* arguments: pointer to the first character in the file, the mode in which to
             open the file
* returns:   the file if it is properly opened. exits otherwise.
* effects:   given file is opened in given mode.
*/
static FILE *open_file(char *fname, char *mode) {
        FILE *fp = fopen(fname, mode); /* open file */

        assert (fp != NULL); /* confirm that file opened correctly */

        return fp; /* return file pointer */
}

#undef T