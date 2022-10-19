/*   
        unblackedges.c
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Removes all black edge pixels from a given PNM file and 
                 outputs the resulting plain PNM file to stdout. Implements 
                 both the UArray2 and Bit2 interfaces.
*/

#include <stdio.h>
#include <stdlib.h>
#include <uarray2.h>
#include <bit2.h>
#include <pnmrdr.h>
#include <assert.h>
#include <string.h>
#include <seq.h>

/* 
        Cell struct
        Purpose: represents a single Cell in our original 2D array image; 
        contains the bit value and whether or not the bit is a black edge
*/
struct Cell {
        int value;      /* bit value: 0 or 1 */
        int col;        /* column index of Bit */
        int row;        /* row index of Bit */
};
typedef struct Cell* T;

FILE *open_file(char *fname);
void remove_edges(int width, int height, Pnmrdr_T reader);
void output_file(Bit2_T pixel_arr, int width, int height);
void find_black_edges(Bit2_T pixel_arr, int width, int height);
void convert_one_bit(Bit2_T pixel_arr, Seq_T stack, int col, int row);
T new_cell(int col, int row, int value);
int neighbor_is_edge(Bit2_T pixel_arr, int col, int row);
void initialize_new(Bit2_T pixel_arr, int width, int height, Pnmrdr_T reader);

const int YES = 0;      /* success value */
const int NO = 1;       /* failure */

/*
* name:         main
* description:  removes all black edge pixels from a given image; starting
*               point for program execution              
* parameters:   int number of command-line arguments, char array containing
                all of the command-line arguments
* returns:      none
*/
int main(int argc, char *argv[]) {
        /* verify correct # of command-line arguments */
        assert(argc == 1 || argc == 2);
        
        FILE *fp;
        if (argc == 1) {
                /* need user input from stdin */
                fp = stdin;
        } else {
                /* open provided file */
                fp = open_file(argv[1]);
                
        }

        /* initialize reader */
        Pnmrdr_T reader = Pnmrdr_new(fp);
        Pnmrdr_mapdata data = Pnmrdr_data(reader);
        
        unsigned type = data.type; /* type of inputted pnm */
        unsigned width = data.width; /* width of inputted pnm */
        unsigned height = data.height; /* height of inputted pnm */
        
        assert(type == 1); /* make sure magic number is P1: pbm format */
        assert(width > 0 && height > 0); /* make sure dimensions nonnegative */
        
        remove_edges(width, height, reader); /* initialize 2D arrays */
        
        Pnmrdr_free(&reader); /* free heap-allocated memory */
        fclose(fp); /* close file */

        return EXIT_SUCCESS;
}

/*
* name:         open_file
* description:  Opens a given pbm file and asserts that it was successfully
*               opened before it is then returned.
* parameters:   the filename (char array) to access for opening that file
* returns:      a pointer to the opened file (of type FILE)
*/
FILE *open_file(char *fname) {
        assert(fname != NULL);
        
        /* open file */
        FILE *fp = fopen(fname, "r"); 

        /* make sure file pointer isn't null */
        assert(fp != NULL); 
        
        return fp; /* return pointer to start of file */
}

/*
* name:         remove_edges
* description:  creates the bit2array and allocates its memory on the heap, 
                then frees it at the end of the function. this serves as a 
                driver function for calling the task-oriented functions 
                including initializing the bit2array values, the actual 
                logic of the program, and writing the result to standard out.
* parameters:   width of bit2array (int), height of bit2array (int),
                and the input's pnm reader
* returns:      none
*/
void remove_edges(int width, int height, Pnmrdr_T reader) {
        /* declare 2D arrays */
        Bit2_T pixel_arr = Bit2_new(width, height);
        
        /* initialize 2D array values */
        initialize_new(pixel_arr, width, height, reader);
        
        /* check outermost ring of  */
        find_black_edges(pixel_arr, width, height);

        /*output PNM file to stdout*/
        output_file(pixel_arr, width, height);

        /* free heap-allocated memory */
        Bit2_free(&pixel_arr);
}

/*
* name:         output_file
* description:  outputs plain pnm to stdout after black edges have been
                removed
* parameters:   2D array (Bit2_T) called pixel_arr, and its respective
                width (int) and height (int)
* returns:      none
*/
void output_file(Bit2_T pixel_arr, int width, int height) {
        /* set magic number of plain pbm file */
        char *magic = "P1"; 

        /* print header */
        fprintf(stdout, "%s\n%d %d\n", magic, width, height); 

        /* loop through pixel_arr */
        for (int r = 0; r < height; r++) {
                for (int c = 0; c < width; c++) {
                        /* print current bit to output file */
                        int curr_bit = Bit2_get(pixel_arr, c, r);
                        fprintf(stdout, "%d ", curr_bit);
                }
                fprintf(stdout, "\n"); /* print new line */
        }
}

/*
* name:         find_black_edges
* description:  finds black edges in original pnm and removes them
* parameters:   2D array (Bit2_T)  called pixel_arr, and its respective
                width (int) and height (int)
* returns:      none
*/
void find_black_edges(Bit2_T pixel_arr, int width, int height) {
        Seq_T stack = Seq_new(width * height); /* initialize stack */
        /* iterate columns */
        for (int r = 0; r < height; r++) {
                /* check bit in first column, current row */
                if (Bit2_get(pixel_arr, 0, r) == 1) {
                        convert_one_bit(pixel_arr, stack, 0, r);
                }
                /* check bit in last column, current row */
                if (Bit2_get(pixel_arr, width - 1, r) == 1) {
                        convert_one_bit(pixel_arr, stack, width - 1, r);
                }
        }
        /* iterate rows */
        for (int c = 1; c < width - 1; c++) { /* subtract the corners */
                /* check bit in current column, first row */
                if (Bit2_get(pixel_arr, c, 0) == 1) {
                        convert_one_bit(pixel_arr, stack, c, 0);
                }
                /* check bit in current column, last row */
                if (Bit2_get(pixel_arr, c, height - 1) == 1) {
                        convert_one_bit(pixel_arr, stack, c, height - 1);
                }
        }
        Seq_free(&stack); /* free heap-allocated memory with stack */
}

/*
* name:         convert_one_bit
* description:  Simulates recursion using a heap-allocated stack and 
                "recursively" adds each black edge pixel and its black neighbors
                to the stack while simultaneously converting the values to
                white.
* parameters:   2D array (Bit2_T) called pixel_arr, a stack implemented as a 
                Hanson sequence, and a specific column index and row index of 
                the 2D Bit2_T array.
* returns:      none
*/
void convert_one_bit(Bit2_T pixel_arr, Seq_T stack, int col, int row) {
        /* create new cell based on current bit */
        T cell = new_cell(col, row, 1);
        Seq_addhi(stack, cell); /* add new cell to stack */

        /* looping through stack of black edges */
        while (Seq_length(stack) > 0) { 
                T cell = Seq_remhi(stack); /* pop top cell of stack */
                int c = cell->col; /* get col and row of curr cell */
                int r = cell->row;
                Bit2_put(pixel_arr, c, r, 0); /* set bit value to 0 */
                free(cell); /* free heap-allocated memory for cell */
                
                /* check top neighbor; add to stack if exists + is black */
                if (neighbor_is_edge(pixel_arr, c, r - 1) == YES) {
                        T cell = new_cell(c, r - 1, 1);
                        Seq_addhi(stack, cell);
                }
                /* check bottom neighbor; add to stack if exists + is black */
                if (neighbor_is_edge(pixel_arr, c, r + 1) == YES) {
                        T cell = new_cell(c, r + 1, 1);
                        Seq_addhi(stack, cell);
                }
                /* check left neighbor; add to stack if exists + is black */
                if (neighbor_is_edge(pixel_arr, c - 1, r) == YES) {
                        T cell = new_cell(c - 1, r, 1);
                        Seq_addhi(stack, cell);
                }
                /* check right neighbor; add to stack if exists + is black */
                if (neighbor_is_edge(pixel_arr, c + 1, r) == YES) {
                        T cell = new_cell(c + 1, r, 1);
                        Seq_addhi(stack, cell);
                }
        }
}

/*
* name:         new_cell
* description:  Initializes a new instance of the Cell struct (T) on the heap,
                as well as its elements (int column and row indices) as well
                as the Bit value stored at the col, row index of the Bit2_T
                2D array.
* parameters:   integer column and row indices and integer bit value.
* returns:      none
*/
T new_cell(int col, int row, int value) {
        T cell = malloc(sizeof(*cell)); /* allocate memory for cell */
        assert(cell != NULL);

        /* intitialize struct values */
        cell->col = col;
        cell->row = row;
        cell->value = value;

        return cell;
}

/*
* name:         neighbor_is_edge
* description:  determines if a neighbor pixel whose row and column indices are
                specified is in bounds of the pixel_arr.
* parameters:   2D Bit2_Array (pixel_arr), and integer col and row indices
                representing the location of the neighbor bit.
* returns:      YES (which equals 0) if specified bit is in bounds, NO 
                (which equals 1) otherwise
*/
int neighbor_is_edge(Bit2_T pixel_arr, int col, int row) {
        /* get width and height of pixel_arr */
        int width = Bit2_width(pixel_arr);
        int height = Bit2_height(pixel_arr);
        
        /* check if neighbor is in bounds & terminate if out of bounds */
        if (col >= 0 && col < width && row >= 0 && row < height) {
                if (Bit2_get(pixel_arr, col, row) == 1) {
                        return YES; /* neighbor has black value */
                }
        }
        return NO;
}

/*
* name:         initialize_new
* description:  loops through a given Bit2_T bit array and intializes each
                element by reading in value from input
* parameters:   2D array (Bit2_T) called pixel_arr, its integer width and height
                dimensions, and an instance of Pnmrdr_T to read input values
* returns:      none
*/
void initialize_new(Bit2_T pixel_arr, int width, int height, Pnmrdr_T reader) {
        /* loop through pixel array */
        for (int r = 0; r < height; r++) {
                for (int c = 0; c < width; c++) {
                        /* read in and funnel value into pixel_arr */
                        Bit2_put(pixel_arr, c, r, (int)Pnmrdr_get(reader));
                }
        }
}