/*
 *     ppmdiff.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *
 *     This program takes in two PPM images and determines the measure of
 *     difference between the two inputted images. Images can be passed in
 *     through the command line or a combination between the command line and
 *     standard input.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "pnm.h"
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"


float ppmdiffer(Pnm_ppm pixmap1, Pnm_ppm pixmap2, A2Methods_T methods, 
                unsigned denominator);
FILE *open_file(char *filename);
bool width_height_diff(Pnm_ppm pixmap1, Pnm_ppm pixmap2);
int min(int num1, int num2);
double pixel_diff(Pnm_rgb pixel1, Pnm_rgb pixel2, unsigned denom);

/* 
 *      name: main
 *   purpose: determine the measure of difference between two images given
 *            via command line or standard input
 *    inputs: argc - number of command line arguments (integer)
 *            argv - char array of command line argumments 
 *   outputs: exit code of EXIT_SUCCESS or EXIT_FAILURE
 *    errors: throws a checked runtime error if the number of command line
 *            arguments is less than or greater than 3. Also throws a CRE if
 *            both "file" inputs are standard input, denoted by a "-".
 */
int main(int argc, char *argv[]) 
{
        assert(argc == 3);
        assert(strcmp(argv[1], "-") != 0 || strcmp(argv[2], "-") != 0);
        
        FILE *file1;
        FILE *file2;

        if (strcmp(argv[1], "-") == 0) { /* first file is standard input */
                file1 = stdin;
                file2 = open_file(argv[2]);
        } else if (strcmp(argv[2], "-") == 0) { /* 2nd file standard input */
                file1 = open_file(argv[1]);
                file2 = stdin;
        } else { /* open both files */
                file1 = open_file(argv[1]);
                file2 = open_file(argv[2]);
        }

        /* get the correct methods suite */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* read in both pixmaps */
        Pnm_ppm pixmap1 = Pnm_ppmread(file1, methods);
        Pnm_ppm pixmap2 = Pnm_ppmread(file2, methods);
        unsigned denom = (pixmap1->denominator + pixmap2->denominator) / 2;

        /* Get + print result to standard output rounded to 4 decimal points */
        printf("%.4f\n", ppmdiffer(pixmap1, pixmap2, methods, denom));

        return EXIT_SUCCESS;
}

/* 
 *      name: open_file
 *   purpose: opens a file for reading given a filename
 *    inputs: filename - the name of the file to be opened
 *   outputs: a file pointer to the beginning of the opened file
 *    errors: throws a checked runtime error if the filename is null or if the
 *            file cannot be opened for reading
 */
FILE *open_file(char *filename) 
{
        assert(filename != NULL);

        FILE *fp = fopen(filename, "r");
        assert(fp != NULL);

        return fp;
}

/* 
 *      name: ppmdiffer
 *   purpose: calculates and returns the difference between the two PNM images
 *    inputs: pixmap1 - the Pnm_ppm instance of the first image
 *            pixmap2 - the Pnm_ppm instance of the second image
 *            methods - the methods suite of the A2Methods UArray2
 *            denom - the maximum grayscale value of the images
 *   outputs: the calculated difference between the two PNM images (type float)
 *    errors: returns 1.0 and prints error message if with width and height
 *            differences between the two images is greater than 1. Throws a
 *            checked runtime error if the height or width are less than or
 *            equal to 0.
 */
float ppmdiffer(Pnm_ppm pixmap1, Pnm_ppm pixmap2, A2Methods_T methods, 
                unsigned denom) 
{
        /* ensure width and height differ by max of 1 */
        if (width_height_diff(pixmap1, pixmap2) == false) {
                fprintf(stderr, "Error: width and height differ by > 1!");
                return 1.0;
        }

        /* find lower width and height */
        int width = min(pixmap1->width, pixmap2->width);
        int height = min(pixmap1->height, pixmap2->height);

        assert(height > 0 && width > 0);
        
        double total = 0.0;

        /* loop through 2D pixel array */
        for (int c = 0; c < width; c++) {
                for (int r = 0; r < height; r++) {
                        /* get the pixels at the current col, row indices */
                        Pnm_rgb pixel1 = methods->at(pixmap1->pixels, c, r);
                        Pnm_rgb pixel2 = methods->at(pixmap2->pixels, c, r);

                        /* add the pixel difference to the total */
                        total += pixel_diff(pixel1, pixel2, denom);
                }
        }
        /* calculate the denominator */
        double denominator = 3 * (double)width * (double)height;

        /* return total difference */
        return sqrt((double)total / denominator);
}

/* 
 *      name: pixel_diff
 *   purpose: calculates the squares of the differences between two
 *            corresponding pixels from the two images
 *    inputs: pixel1 - One RGB pixel from the first image (of type Pnm_rgb)
 *            pixel2 - Corresponding RGB pixel from the second image (of type
 *            Pnm_rgb)
 *            denom - the maximum grayscale value in the image (integer)
 *   outputs: the squares of the differences between the two pixels from the
 *            two corresponding images (of type double)
 *    errors: none
 */
double pixel_diff(Pnm_rgb pixel1, Pnm_rgb pixel2, unsigned denom) 
{
        /* get RGB values from pixels */
        int red1 = pixel1->red;
        int red2 = pixel2->red;
        int green1 = pixel1->green;
        int green2 = pixel2->green;
        int blue1 = pixel1->blue;
        int blue2 = pixel2->blue;
        
        /* calculate the differences between each pixel */
        double red_diff = (double)abs(red1 - red2) / denom;
        double green_diff = (double)abs(green1 - green2) / denom;
        double blue_diff = (double)abs(blue1 - blue2) / denom;

        /* calculate the square of the differences */
        double diff = (red_diff * red_diff) + (green_diff * green_diff) + 
                (blue_diff * blue_diff);
        
        return diff;
}

/* 
 *      name: width_height_diff
 *   purpose: determines if the difference in dimensions between the two
 *            inputted images is too big
 *    inputs: pixmap1 - the Pnm_ppm instance of the first image
 *            pixmap2 - the Pnm_ppm instance of the second image
 *   outputs: false if the widths and heights of I and I prime differ by more
 *            than 1; true otherwise
 *    errors: raises a checked runtime error if the difference in height is
 *            greater than 1 or the difference in width is greater than 1
 */
bool width_height_diff(Pnm_ppm pixmap1, Pnm_ppm pixmap2) 
{
        /* get width and height from corresponding Pnm_ppm pixmaps */
        int width1 = pixmap1->width;
        int width2 = pixmap2->width;
        int height1 = pixmap1->height;
        int height2 = pixmap2->height;
        
        /* verify that width and height of I and I prime differ by max 1 */
        if (abs(width1 - width2) > 1 || abs(height1 - height2) > 1) {
                return false;
        }
        return true;
}

/* 
 *      name: min
 *   purpose: returns the minimum of two integers
 *    inputs: num1 - the first integer to be compared
 *            num2 - the second integer to be compared
 *   outputs: the minimum of the two integers
 *    errors: none
 */
int min(int num1, int num2) 
{
        if (num1 < num2) {
                return num1;
        } 
        return num2;
}