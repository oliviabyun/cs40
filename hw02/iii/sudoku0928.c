/*   
        sudoku.c
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Implementing a 2D array from the UArray2 interface
                 in order to check if a sudoku puzzle is solved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <uarray2.h>
#include <assert.h>

/* Function Declarations */
FILE *open_file(char *filename);
char *get_user_file();
void initialize_sudoku_board(UArray2_T sudoku_board, FILE *fp);
int all_subarrays_valid(UArray2_T sudoku_board);
int one_subarray_valid(int col_min, int col_max, int row_min, int row_max, 
                       UArray2_T sudoku_board);
int sudoku_is_solved(UArray2_T sudoku_board);
void is_valid_intensity(int col, int row, UArray2_T sudoku_board,
                                void *value, void *cl);
void verify_no_duplicates(int col, int row, UArray2_T sudoku_board,
                                void *value, void *cl);


//0 is true, 1 is false
const int DIM = 9; /* dimension of Sudoku board */
const int SUCCESS = 0; /* success value */
const int FAILURE = 1; /* failure */

/* 
   houses data necessary for properly iterating through
   each line of the sudoku board
*/

struct Tracker {
        int *cell_arr; /* array tracking of elements/duplicates in a line */
        int count;     /* track # of elements added to cell_arr */
        int result;    /* SUCCESS if no duplicates, FAILURE if duplicates exist */
};
typedef struct Tracker* T;

//#define T Tracker

// #define T Tracker;

/*
* name:         
* description:  
* parameters:   
* returns:      
*/
int main(int argc, char *argv[]) {
        UArray2_T sudoku_board = UArray2_new(DIM, DIM, sizeof(int));
        
        assert(argc == 1 || argc == 2);

        char *fname = NULL;


        if (argc == 1) {
                //TODO: read from user from stdin
                //argv[1]
                //funnel user input into a file then call open_file to handle logic
                //FILE *fp = function() //reads in input and returns file
                fname = get_user_file();
                //FILE *fp = open_file(fname);
        } else { // argc = 2
                //FILE *fp = open_file(argv[1]);
                fname = argv[1];
        }
        FILE *fp = open_file(fname);

        initialize_sudoku_board(sudoku_board, fp);

        //TODO FREE MEMORY
        
        int result = sudoku_is_solved(sudoku_board); 
        UArray2_free(&sudoku_board);

        return result;
        
}

char *get_user_file() {
        //FILE *fp = fopen("raw.pgm", "w");
        char *filename = "input.pgm";
        FILE *fp = fopen(filename, "w");
        
        /* accounting for NUL character at end of string */
        int size = 3 * sizeof(char);
        char *magic_number = (char *) malloc(size);        
        fgets(magic_number, size, stdin);
        fprintf(stderr, "*** %s ***\n", magic_number);
        
        int width = fgetc(stdin);
        int height = fgetc(stdin);
        int gray = fgetc(stdin);

        fprintf(stderr, "width: %c, height: %c, gray: %c \n", 
                                                width, height, gray);
        fprintf(stdout, "%s\n%u %u\n%u\n", magic_number, width, height, gray);
        // fprintf(fp, "%s\n%u %u\n%u\n", magic_number, width, height, gray);
        
        

        //int c; //c c 
        //TODO: make sure we don't cut off any characters
        // while ((c = getchar()) != EOF) {
        //         //c = fgetc(fp);
        //         //fprintf("%c", c);
        //         fputc(c, fp);
        //         //c = fgetc(fp);
        //         //fputc
        // }
        fclose(fp);

        return filename;
        
}

/*
* name:     initialize_sudoku_board    
* description:  
* parameters:   
* returns:      
*/
void initialize_sudoku_board(UArray2_T sudoku_board, FILE *fp) {
        /* initialize Pnmrdr reader and data struct */
        Pnmrdr_T reader = Pnmrdr_new(fp);
        Pnmrdr_mapdata data = Pnmrdr_data(reader);
        
        /* initialize variables containing info about pgm */
        unsigned width = data.width;
        unsigned height = data.height;
        unsigned gray = data.denominator; //check if > 9
        
        /* Checking max val is within upper bound: should be <= 9 */
        assert(gray <= 9 && width == 9 && height == 9);

        /* Filling up Sudoku board from file input */
        for (unsigned i = 0; i < width; i++) {
                for (unsigned j = 0; j < height; j++) {
                        //int val = Pnmrdr_get(reader); 
                        //printf("val = %d\n", val);
                        //*(int *) UArray2_at(sudoku_board, i, j) = val;
                        *(int *) UArray2_at(sudoku_board, i, j) = Pnmrdr_get(reader);
                }
        }

        Pnmrdr_free(&reader);
}

/*
* name:        sudoku_is_solved 
* description:  
* parameters:   
* returns:      
*/
int sudoku_is_solved(UArray2_T sudoku_board) {
        /* verify that each cell is in bounds (1-9) */
        int intensity_result = SUCCESS;
        UArray2_map_row_major(sudoku_board, is_valid_intensity, &intensity_result);

        if (intensity_result == FAILURE) {
                printf("failure\n");
                return FAILURE;
        }

        //TODO: call functions to check for duplicates in row and col
        T tracker =  malloc(sizeof(*tracker));
        int *cell_arr = (int *) malloc(sizeof(int) * DIM);

        /* put into helper func l8r bc just reseting contents to 0 */
        for (int i = 0; i < DIM; i++) {
                cell_arr[i] = 0;
        }
        
        (tracker)->cell_arr = cell_arr;
        (tracker)->count = 0;
        (tracker)->result = SUCCESS; /* SUCCESS until finding duplicate */
        
        /* check for duplicates in columns */
        UArray2_map_col_major(sudoku_board, verify_no_duplicates, tracker);
        /* check for duplicates in rows */
        UArray2_map_row_major(sudoku_board, verify_no_duplicates, tracker);
        int row_dup_result = (tracker)->result;
        printf("the row result is: %d\n", row_dup_result);
        
        /* verify that 3x3 grids don't have duplicates */
        int result = all_subarrays_valid(sudoku_board);
        printf("The result is: %d\n", result);

        free(tracker);
        free(cell_arr);
        
        return result;
}

/*
* name:         verify_no_duplicates
* description:  
* parameters:   
* returns:      
*/
void verify_no_duplicates(int col, int row, UArray2_T sudoku_board,
                                void *value, void *cl) {
        (void)col;
        (void)row;
        (void)sudoku_board;
       
        T tracker = cl;
        /*int *cell_arr = tracker->cell_arr;
        int count = tracker->count;
        int result = tracker->result;*/
        //value is 1-9
        //index is value - 1 so 0-8
        int index = *(int *) value - 1; //if val is 1 then index should be 0
        if (tracker->cell_arr[index] != 0) { /* check for duplicate */
                tracker->result = FAILURE; /* duplicate found */
        } else { /* otherwise set to 1 bc 1 instance of curr value so far */
                tracker->cell_arr[index] = 1;
        }
        tracker->count++; /* increment # of elements added in line */
        
        /* check if we've added each element of a line */
        if (tracker->count == 9) {
                /* in a new line: reset cell_arr and reset count to 0 */
                for (int i = 0; i < DIM; i++) {
                        tracker->cell_arr[i] = 0;
                }
                tracker->count = 0;
        }

        //funnel values into respective indices of cell_arr
        //check cell_arr element based on index for duplicate
                //if duplicate, return 1
        //else continue funneling
}

/*
* name:         is_valid_intensity    
* description:  
* parameters:   
* returns:      
* note: this isn't the most efficient because if we encounter
*       an out of bounds value, we still carry out the rest of
*       the program. but since its only 9x9, we assume it isn't
*       affecting the efficiency too much overall.
*/
void is_valid_intensity(int col, int row, UArray2_T sudoku_board,
                                void *p, void *cl) {
        (void)p;
        /* get value of current cell in sudoku board */
        int curr_value = *(int *) UArray2_at(sudoku_board, col, row);

        /* verify cell is in bounds: between 1-9, and not already failure */
        if (curr_value > 0 && curr_value <= DIM && *(int *)cl != FAILURE) {
                *(int *)cl = SUCCESS; /* cell is in bounds */
        } else {
                *(int *)cl = FAILURE; /* cell is out of bounds */
        }
}

/*
* name:         
* description:  
* parameters:   
* returns:      
*/
// void has_duplicates() {
        
// }


FILE *open_file(char *fname) {
        FILE *fp = fopen(fname, "r");
        assert (fp != NULL);
        
        return fp;
}

/* TODO: write  */

int all_subarrays_valid(UArray2_T sudoku_board) {
        int col_min = 0, row_min = 0;
        int col_max = 2, row_max = 2;
        int count = 0, is_valid = 0;

        for (int i = 0; i < 9; i++) {
                count++; /* increment count */
                
                // printf("i = %d, col_min = %d, col_max = %d, row_min = %d, row_max = %d, count = %d\n", 
                //                                         i, col_min, col_max, row_min, row_max, count);
                is_valid = one_subarray_valid(col_min, col_max, row_min, 
                                        row_max, sudoku_board);
                
                col_min += 3;
                col_max += 3;

                if (count == 3) {
                        count = 0;
                        row_min += 3;
                        row_max += 3;
                        col_min = 0;
                        col_max = 2;
                }

                if (is_valid == FAILURE) {
                        return is_valid; /* return failure */
                }
        }
        return is_valid; /* return success */
}

/* TODO: change function name later */
 /* get element at each index and add value 
to corresponding part of cell_arr */
int one_subarray_valid(int col_min, int col_max, int row_min, int row_max, 
                       UArray2_T sudoku_board) {
        int *cell_arr = (int *) malloc(sizeof(int) * DIM);

        for (int i = 0; i < DIM; i++) {
                cell_arr[i] = 0;
        }
        
        /* loop through one grid */
        for (int r = row_min; r <= row_max; r++) {
                for (int c = col_min; c <= col_max; c++) {
                        /* get value of current cell in 3x3 grid */
                        int *curr_cell = (int *) UArray2_at(sudoku_board, c, r);
                        /* calculate index of cell_arr */
                        int index = *curr_cell - 1;

                        /* check for duplicate values */
                        if (cell_arr[index] != 0) {
                                free(cell_arr);
                                return FAILURE;
                        }
                        /*add curr_cell to correct index of cell_arr */
                        cell_arr[index] = 1;
                }
        }
        free(cell_arr);
        return SUCCESS;
}


