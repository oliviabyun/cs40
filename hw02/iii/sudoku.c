/*   
        sudoku.c
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Checks whether a sudoku puzzles is correctly solved or not
                 by implementing a 2D array from the UArray2 interface.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <uarray2.h>
#include <assert.h>

/* Function Declarations */
void initialize_sudoku_board(UArray2_T sudoku_board, FILE *fp);
int sudoku_is_solved(UArray2_T sudoku_board);
void reset_array(int **cell_arr);
void verify_no_duplicates(int col, int row, UArray2_T sudoku_board,
                                void *value, void *cl);
void is_valid_intensity(int col, int row, UArray2_T sudoku_board,
                                void *value, void *cl);
FILE *open_file(char *filename);
int all_subarrays_valid(UArray2_T sudoku_board);
int one_subarray_valid(int col_min, int col_max, int row_min, int row_max, 
                       UArray2_T sudoku_board);

const int DIM = 9;      /* dimension of Sudoku board */
const int SUCCESS = 0;  /* success value */
const int FAILURE = 1;  /* failure */

/* 
        Tracker struct
        description: Houses data necessary for properly iterating through 
        each line of the Sudoku board
*/
struct Tracker {
        int *cell_arr; /* array tracking of elements/duplicates in a line */
        int count; /* track # of elements added to cell_arr */
        int result; /* SUCCESS if no duplicates, FAILURE if duplicates exist */
};
typedef struct Tracker* T;


/*
* name:         main
* description:  determines if a sudoku board is correctly solved; uses a 9x9
                2D array to store each cell value; starting point for program
                execution
* parameters:   int number of command-line arguments, char array containing
                all of the command-line arguments
* returns:      0 if puzzle successfully solved, 1 otherwise
*/
int main(int argc, char *argv[]) {
        /* initialize new 9x9 sudoku board */
        UArray2_T sudoku_board = UArray2_new(DIM, DIM, sizeof(int));
        
        /* verify correct # of command-line arguments */
        assert(argc == 1 || argc == 2); 
        
        if (argc == 1) {
                /* initialize board from standard (user) input */
                initialize_sudoku_board(sudoku_board, stdin);
        } else {
                /* open provided file and initialize board*/
                FILE *fp = open_file(argv[1]);
                initialize_sudoku_board(sudoku_board, fp);
                fclose(fp); /* close file */
        }
        
        /* get overall result */
        int result = sudoku_is_solved(sudoku_board); 

        UArray2_free(&sudoku_board); /* free heap-allocated memory */

        return result;
}

/*
* name:         initialize_sudoku_board    
* description:  initializes each cell of sudoku board with values supplied by
                input, verifies that dimensions and max grayscale val is correct
* parameters:   2D array sudoku board of type UArray2_T
* returns:      none
*/
void initialize_sudoku_board(UArray2_T sudoku_board, FILE *fp) {
        assert (fp != NULL);
        /* initialize Pnmrdr reader and data struct */
        Pnmrdr_T reader = Pnmrdr_new(fp);
        Pnmrdr_mapdata data = Pnmrdr_data(reader);
        
        unsigned width = data.width; /* height of inputted pgm */
        unsigned height = data.height; /* width of inputted pgm */
        unsigned gray = data.denominator; /* maximum grayscale value */
        unsigned type = data.type;

        /* Verify dimensions, maximum grayscale val is 9*/
        assert(gray == 9 && width == 9 && height == 9 && type == 2);

        /* Filling up Sudoku board from file input */
        for (unsigned i = 0; i < width; i++) {
                for (unsigned j = 0; j < height; j++) {
                        /* initialize current cell */
                        *(int *) UArray2_at(sudoku_board, i, j) = 
                                Pnmrdr_get(reader); 
                }
        }
        Pnmrdr_free(&reader); /* free heap-allocated memory */
}

/*
* name:         sudoku_is_solved 
* description:  determines if puzzle is correctly solved by verifying in-bound
                values and verifying that there are no duplicates in any rows,
                columns, or respective 3x3 grids
* parameters:   2DArray sudoku board of type UArray2_T
* returns:      SUCCESS if solved, FAILURE otherwise (if duplicates found or 
                any cells are out-of-bounds)
*/
int sudoku_is_solved(UArray2_T sudoku_board) {
        int intensity_result = SUCCESS;
        UArray2_map_row_major(sudoku_board, is_valid_intensity, 
                              &intensity_result); /* verify in-bounds cells */
        
        if (intensity_result == FAILURE) {
                return FAILURE; /* found out-of-bounds cell */
        }
        /* initialize struct as closure for duplicate-finding functions */
        T tracker =  malloc(sizeof(*tracker));
        assert(tracker != NULL);
        int *cell_arr = (int *) malloc(sizeof(int) * DIM);
        assert(cell_arr != NULL);
        
        reset_array(&cell_arr); /* set all elements to 0 */
        (tracker)->cell_arr = cell_arr; /* initialize cell_arr */
        (tracker)->count = 0; /* initialize element counter to 0 */
        (tracker)->result = SUCCESS; /* SUCCESS until finding duplicate */
        
        /* check for duplicates in columns */
        UArray2_map_col_major(sudoku_board, verify_no_duplicates, tracker);
        int col_dup_result = (tracker)->result;
        
        /* check for duplicates in rows */
        UArray2_map_row_major(sudoku_board, verify_no_duplicates, tracker);
        int row_dup_result = (tracker)->result;
        
        /* verify that 3x3 grids don't have duplicates */
        int sub_dup_result = all_subarrays_valid(sudoku_board);

        free(tracker); /* free heap-allocated memory */
        free(cell_arr);

        if (col_dup_result == FAILURE || row_dup_result == FAILURE || 
            sub_dup_result == FAILURE) {
                return FAILURE; /* duplicate was found */
        }
        return SUCCESS; /* all win conditions met: success! */
}

/*
* name:         reset_array
* description:  sets each value of given integer array to 0; used for 
                initializing or resetting an array
* parameters:   pointer to integer array storing values in a given line
* returns:      none
*/
void reset_array(int **cell_arr) {
        assert(*cell_arr != NULL);
        /* loop through array */
        for (int i = 0; i < DIM; i++) {
                (*cell_arr)[i] = 0;
        }
}

/*
* name:         verify_no_duplicates
* description:  This serves as an apply function called on each element in the
                2D array sudoku board to verify that there are no duplicates in
                a given line (can be a row or column).
* parameters:   column of sudoku_board (integer), row of sudoku_board 
                (integer), 2D array sudoku board of type UArray2_T, pointer to
                value of current element, and a pointer to a closure - in this
                case, a struct containing an integer array, an integer counter, 
                and integer result that helps keep track of data needed for
                current operation (see Tracker struct for details)
* returns:      none   
*/
void verify_no_duplicates(int col, int row, UArray2_T sudoku_board,
                                void *value, void *cl) {
        (void)col; /* cast unused elements to type void */
        (void)row;
        (void)sudoku_board;
        assert(value != NULL && cl != NULL);
       
        T tracker = cl; /* cast closure to Tracker struct */
        int index = *(int *) value - 1; /* map each value to spot in array */
        
        /* check for duplicate value in current index */
        if (tracker->cell_arr[index] != 0) {
                tracker->result = FAILURE; /* duplicate found */
        } else { /* 1 instance of curr value so far: set to 1 */
                tracker->cell_arr[index] = 1;
        }
        tracker->count++; /* increment # of elements added in line */
        
        /* check if we've added every single element of a line */
        if (tracker->count == 9) {
                /* in a new line: reset cell_arr and reset count to 0 */
                reset_array(&(tracker->cell_arr));
                tracker->count = 0;
        }
}

/*
* name:         is_valid_intensity    
* description:  This serves as the apply function for determining whether or not
*               the elements of the 2D array sudoku board are in bounds 
*               (between 1 and 9). If a given element is out of the specified,
*               bounds, the function updates the *cl pointer to keep track of
*               success or failure.
* parameters:   column of 2D array (int), row of 2D array (int), 2D array 
*               sudoku_board (UArray2_T), void pointer called *p which points to
*               element at the index [col][row] of 2D array, void pointer called
*               *cl and we use *cl as a pointer to an integer value (0 or 1), 
*               which represents a SUCCESS or FAILURE,respectively, in order to 
*               update the rest of the program of a potential failure.
* returns:      none
* note: if we encounter an out of bounds value, we still carry 
*       out the rest of the program. but since its only 9x9, 
*       we assume it isn't affecting the efficiency too much overall.
*/
void is_valid_intensity(int col, int row, UArray2_T sudoku_board,
                                void *p, void *cl) {
        (void)p;
        assert(cl != NULL);
        /* get value of current cell in sudoku board */
        int curr_value = *(int *) UArray2_at(sudoku_board, col, row);

        /* verify cell is in bounds: between 1-9, and not already failure */
        assert(curr_value > 0 && curr_value <= DIM && *(int *)cl != FAILURE);
}

/*
* name:         open_file
* description:  Opens the pgm file and asserts that it was successfully
*               opened before it is then returned.
* parameters:   the filename to access for opening that file
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
* name:         all_subarrays_valid
* description:  Initializes the column and row bounds for iterating through
*               the 3x3 subarrays of the sudoku board. Calls one_subarray_valid
*               which returns SUCCESS/FAILURE, which this function simply passes
*               along as its own return function.
* parameters:   2D array sudoku board of UArray2_T type
* returns:      the return value from one_subarray_valid, which is either 
*               success (0) or failure (1) depending on whether that function
*               found duplicates or not.
*/
int all_subarrays_valid(UArray2_T sudoku_board) {
        /* Initializing specified bounds */
        int col_min = 0, row_min = 0;
        int col_max = 2, row_max = 2;
        int count = 0, is_valid = 0;

        /* Iterating 9 times to account for each element of 3x3 grid */
        for (int i = 0; i < 9; i++) {
                count++; /* increment count */
                
                /* determine if current 3x3 grid has duplicates */
                is_valid = one_subarray_valid(col_min, col_max, row_min, 
                                        row_max, sudoku_board);
                
                col_min += 3; /* increment column indices */
                col_max += 3;
                
                /* once we reach end of line (after 3 subarrays) */
                if (count == 3) {
                        count = 0; /* reset element count */
                        row_min += 3; /* increment row indices */
                        row_max += 3;
                        col_min = 0; /* reset column indices */
                        col_max = 2;
                }
                if (is_valid == FAILURE) {
                        return is_valid; /* found duplicate: return failure */
                }
        }
        return is_valid; /* return success */
}

/*
* name:         one_subarray_valid
* description:  Get element at each index of the sudoku board and update 
                cell_arr to keep track of their respective frequencies to track
                duplicates. Cell_arr sets each element to zero and is updated 
                accordingly based on if duplicate.
* parameters:   minimum column of 2D array (int), maximum column of 2D 
                array (int), minimum row of 2D array (int), maximum row of 2D 
                array (int), 2D array sudoku_board (UArray2_T).
* returns:      default return to success, returns failure if a duplicate is 
*               found in the 3x3 grid
*/
int one_subarray_valid(int col_min, int col_max, int row_min, int row_max, 
                       UArray2_T sudoku_board) {
        /* allocate memory for integer array to track duplicate cell values */
        int *cell_arr = (int *) malloc(sizeof(int) * DIM);
        assert(cell_arr != NULL);
        reset_array(&cell_arr); /* reset array to zero values */
        
        /* loop through one grid */
        for (int r = row_min; r <= row_max; r++) {
                for (int c = col_min; c <= col_max; c++) {
                        /* get value of current cell in 3x3 grid */
                        int *curr_cell = 
                                (int *) UArray2_at(sudoku_board, c, r);
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
        free(cell_arr); /* free heap-allocated memory */
        return SUCCESS;
}


