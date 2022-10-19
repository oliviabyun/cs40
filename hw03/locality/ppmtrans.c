/*
 *     ppmtrans.c
 *     by Olivia Byun (obyun01) and Alyssa Williams (awilli36)
 *     10/13/2022
 *     HW3: locality
 *
 *     Summary purpose: 
 *     ppmstrans takes in a ppm image through a file or through the commandline
 *     and applies a transformation from these options (chosen through 
 *     commandline flags):
 *              rotate 0, 90, 180, or 270 degrees clockwise
 *              flip horizontally (left to right) or vertically (up to down)
 *              transpose (across UL to LR axis)
 *     It can do these rotations from a choice of different mapping functions:
 *              row major
 *              column major
 *              block major
 *     It can also take in a file to write timing data to -- how long it takes
 *     to rotate the image using the mapping function (not the time of the 
 *     whole program, just the rotating part).
 *     Outputs the result to stdin
 *              
 *     Use: 
 *     Useful for editing images by transforming them. Can also be used 
 *     experimentally with the timing data to evaluate the efficiency of each
 *     type of rotation with each type of mapping functions, which is an 
 *     interesting way to learn about cache times. 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pnmrdr.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "rotate.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

/*
 *        name: open_file
 * description: given a file name, opens the file and returns pointer to the
 *              start of the file
 *  parameters: *filename - string name of file
 *     returns: pointer to beginning of file
 *      errors: throws a checked runtime error if the filename is null or if
 *              the file is unable to be opened
 */
FILE *open_file(char *filename) 
{
        assert (filename != NULL);

        FILE *fp = fopen(filename, "r"); /* open file for reading */

        assert (fp != NULL);

        return fp;
}

/*
 *        name: print_time_file
 * description: prints data about image and rotation time to supplied time file
 *  parameters: *time_file - pointer to 
 *                rotation - degree rotation that was performed on the image
 *               *map_name - the name of the mapping function 
 *                   width - the width of the original image
 *                  height - the height of the original image
 *               time_used - the time taken to rotate the image, in nanoseconds
 *              pixel_time - the average time taken to rotate each pixel, in
 *                           nanoseconds
 *     returns: none
 *      errors: throws a checked runtime error if time_file or map_name are
 *              NULL
 */
void print_time_file(FILE *time_file, int rotation, char *map_name, int width,
                        int height, double time_used, double pixel_time)
{
        assert (time_file != NULL && map_name != NULL);        
        fprintf(time_file, "Rotation: %d", rotation);
        fprintf(time_file, ", Mapping: %s", map_name);
        fprintf(time_file, ", Dimensions: %dx%d", width, height);
        fprintf(time_file, ", Rotate time: %f nanoseconds", time_used);
        fprintf(time_file, ", Rotate time per pixel: %f nanoseconds\n",
                pixel_time);
}

/*
 *        name: translate_image
 * description: Sets up the pixmap for translating the image, then applies the 
 *              transformation Also handles the timing option. 
 *  parameters:             fp - the file stream
 *                     methods - the A2Methods 
 *                    rotation - the type of transformation to apply
 *              time_file_name - the name of the timing file
 *                         map - the mapping function to use 
 *                    map_name - the name of the mapping function 
 *     returns: None
 *      errors: throws a checked runtime error if the Pnm_ppm pixmap is NULL
 */
void translate_image(FILE *fp, A2Methods_T methods, int rotation, 
                     char *time_file_name, A2Methods_mapfun *map, 
                     char *map_name)
{
        Pnm_ppm pixmap = Pnm_ppmread(fp, methods);
        assert(pixmap != NULL);

        A2Methods_UArray2 source = pixmap->pixels;

        unsigned width = pixmap->width;
        unsigned height = pixmap->height;
        
        if (rotation == 90 || rotation == 270 || rotation == TRANSPOSE) {
                /* due to rotation, width and height are swapped */
                width = pixmap->height;
                height = pixmap->width;
                pixmap->width = width;
                pixmap->height = height;        
        }
        
        /* create new 2D array based on dimensions */
        pixmap->pixels = methods->new(width, height, sizeof(struct Pnm_rgb));

        if (time_file_name != NULL) {
                /* open timing file for appending */
                FILE *time_file = fopen(time_file_name, "a"); 
                assert(time_file != NULL);

                /* create and start timer */
                CPUTime_T timer = CPUTime_New();
                CPUTime_Start(timer);

                rotate(rotation, source, pixmap, map); /* call rotation */

                /* get and calculate time for rotation, time per pixel */
                double time_used = CPUTime_Stop(timer);
                double pixel_time = time_used / (width * height);

                /* print data to timing file */
                print_time_file(time_file, rotation, map_name, width, height, 
                                time_used, pixel_time);
                
                CPUTime_Free(&timer); /* free memory associated with timer */
        } else {
                /* rotate image with no timing data */
                rotate(rotation, source, pixmap, map);
        }

        Pnm_ppmwrite(stdout, pixmap); /* write image data to standard out */

        /* free heap-allocated memory associated with pixmap and source */
        Pnm_ppmfree(&pixmap);
        methods->free(&source);
}

/*
 *        name: main
 * description: Main executable function 
 *  parameters: argc - number of arguments given on commandline
 *              argv - the arguments given on commandline
 *     returns: EXIT_SUCCESS if the program was successful
 *      errors: Throws a CRE if there is an error opening the file 
 */
int main(int argc, char *argv[]) 
{
        FILE *fp;
        bool file_read = false;
        
        char *time_file_name = NULL;
        
        int   rotation       = 0;
        int   i;
        
        char *map_name = "col-major";

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);

        for (i = 1; i < argc; i++) { //./ppmtrans file.ppm -row-major
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                        map_name = "row-major";
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                        map_name = "column-major";
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                        map_name = "block-major";
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {
                                usage(argv[0]);
                        }
                        char *flip_name = argv[++i];
                        if (strcmp(flip_name, "horizontal") == 0) {
                                rotation = HORIZONTAL;
                        } else if (strcmp(flip_name, "vertical") == 0) {
                                rotation = VERTICAL;
                        } else {
                                fprintf(stderr, "Flip must be horizontal or");
                                fprintf(stderr, " vertical\n");
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        rotation = TRANSPOSE;
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (i == argc - 1) {
                        fp = open_file(argv[argc - 1]);
                        file_read = true;
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        if (!file_read) {
                fp = stdin; /* read from standard input if no file provided */
        }
        
        /* call translation with appropriate parameter values */
        translate_image(fp, methods, rotation, time_file_name, map, map_name);
        
        fclose(fp); /* close file */

        return EXIT_SUCCESS;
}


