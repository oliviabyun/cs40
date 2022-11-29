/* um.c
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This program is a Universal Machine (UM), which umulates an imaginary 
 * hardware, complete with its own registers and segmented memory. It reads in
 * instructions in the UM language in the .um file given on the command line. 
 * These instructions can be any of the 14 UM operations. 
 *
 * This file in particular handles the file from the command line and reads in 
 * the instructions for execution, calling functions from other files for the
 * execution of each instruction. 
 */

#include "calculate.h"
#include "memory.h"
#include "perform_io.h"
#include "mem.h"
#include "seq.h"
#include "assert.h"
#include "uarray.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "bitpack.h"
#include <stdbool.h>
#include <sys/stat.h>

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV, NAND, HALT, ACTIVATE, 
        INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/* 
 *      name: free_all
 *   purpose: frees all of the data structures stored on the heap
 *    inputs:        registers - pointer to the array of registers
 *                    segments - pointer to the sequence of segments 
 *           unmapped_segments - pointer to the sequence of unmapped segments 
 *   outputs: none
 *    errors: CRE if registers, segments, or unmapped_segments is NULL
 */
void free_all(uint64_t *registers, Seq_T segments, Seq_T unmapped_segments)
{
        assert(registers != NULL);
        assert(segments != NULL);
        assert(unmapped_segments != NULL);
        
        /* free registers and unmapped segments */
        FREE(registers);
        Seq_free(&unmapped_segments);
        
        int count = 1;
        /* free each individual segment if not yet freed */
        while (Seq_length(segments) != 0) {
                UArray_T seg = Seq_remlo(segments);
                if (seg != NULL) {
                        UArray_free(&seg);
                }
                count++;
        }
        
        /* free segments */
        Seq_free(&segments);
}

/* 
 *      name: setup_seg0
 *   purpose: reads from the file and stores all of the instructions in memory
 *    inputs:        fp - the file pointer to read from
 *            num_words - the number of instructions in the file
 *   outputs: none
 *    errors: throws a CRE if the file pointer or segments are NULL
 */
UArray_T setup_seg0(FILE *fp, int num_words)
{
        assert(fp != NULL);
        
        /* allocate space for segment 0 */
        UArray_T seg0 = UArray_new(num_words, sizeof(uint64_t));

        uint64_t word = 0;
        uint64_t character;

        /* loop through segment 0 */
        for (int i = 0; i < num_words; i++) {
                /* initialize each word from input file */
                for (int j = 0; j < 4; j++) {
                        character = getc(fp);
                        word = Bitpack_newu(word, 8, 24 - (j * 8), character);
                }
                *(uint64_t *)UArray_at(seg0, i) = word;
        }

        return seg0;
}

/* 
 *      name: run_program
 *   purpose: executes the program
 *    inputs: seg0 - segment 0 in memory, which contains the program code to
 *                   be executed (type UArray_T)
 *   outputs: none
 *    errors: unchecked runtime error if program counter points to a word that
 *            doesn't code for a valid instruction, or if the program counter
 *            points out of bounds of $m[0]
 *            CRE if seg0 is NULL
 */
void run_program(UArray_T seg0)
{
        assert(seg0 != NULL);
        
        /* create and initialize registers */
        uint64_t *registers = (uint64_t *)ALLOC(sizeof(uint64_t) * 8);
        for (int i = 0; i < 8; i++) {
                registers[i] = 0;
        }

        /* initialize segments and unmapped segments */
        Seq_T segments = Seq_new(20); 
        Seq_T unmapped_segments = Seq_new(20);
        Seq_addhi(segments, seg0);

        bool halted = false;
        int next_seg = 1; /* keep track of next available segment */
        int program_idx = 0; /* current index in segment 0 */
        
        while (!halted) {
                /* get segment 0 */
                UArray_T segment0 = (UArray_T)Seq_get(segments, 0);
                assert(segment0 != NULL);

                /* get current word in segment 0 and increment program index */
                uint64_t word = *(uint64_t *)UArray_at(segment0, program_idx);
                program_idx++;

                /* unpack op code and registers from word */
                unsigned op = Bitpack_getu(word, 4, 28);
                unsigned ra = Bitpack_getu(word, 3, 6);
                unsigned rb = Bitpack_getu(word, 3, 3);
                unsigned rc = Bitpack_getu(word, 3, 0);

                if (op == HALT) {
                        halted = true;
                } else if (op == CMOV) {
                        conditional_move(ra, rb, rc, registers);
                } else if (op == SLOAD) {
                        segmented_load(ra, registers[rb], registers[rc], 
                                       registers, segments);
                } else if (op == SSTORE) {
                        segmented_store(registers[ra], registers[rb], 
                                        registers[rc], segments);
                } else if (op == ADD) {
                        add(ra, rb, rc, registers);
                } else if (op == MUL) {
                        multiply(ra, rb, rc, registers);
                } else if (op == DIV) {
                        divide(ra, rb, rc, registers);
                } else if (op == NAND) {
                        bitwise_NAND(ra, rb, rc, registers);
                } else if (op == ACTIVATE) {
                        map_segment(rb, registers[rc], registers, segments, 
                                    unmapped_segments, &next_seg);
                        /* Make sure memory resources haven't been exhausted */
                        uint32_t limit = ~0;
                        if ((uint32_t)next_seg >= limit) {
                                halted = true;
                        }
                } else if (op == INACTIVATE) {
                        unmap_segment(registers[rc], segments, 
                                      unmapped_segments);
                } else if (op == OUT) {
                        output(registers[rc]);
                } else if (op == IN) {
                        input(rc, registers);
                        if ((int) registers[rc] == ~0) {
                                halted = true;
                        }
                } else if (op == LOADP) {
                        load_program(registers[rb], segments);
                        program_idx = registers[rc];
                } else if (op == LV) {
                        ra = Bitpack_getu(word, 3, 25);
                        unsigned val = Bitpack_getu(word, 25, 0);
                        load_value(ra, val, registers);
                } else {
                        halted = true;
                }
        }
        free_all(registers, segments, unmapped_segments); /* free memory */
}

/* 
 *      name: main
 *   purpose: reads in a provided file and starts the program
 *    inputs: argc - the number of command line arguments (integer)
 *            argv - array of the command line arguments
 *   outputs: EXIT_FAILURE if command line arguments is not 2, EXIT_SUCCESS
 *            otherwise
 *            EXIT_SUCCESS if program runs without errors
 *    errors: none
 */
int main(int argc, char *argv[]) 
{
        /* make sure one file is provided to the program */
        if(argc != 2) {
                fprintf(stderr, "ERROR: incorrect format\n");
                return EXIT_FAILURE;
        }

        /* determine length of input file and calculate number of words */
        struct stat st;
        assert(stat(argv[1], &st) == 0);
        uint64_t size = st.st_size;
        int num_words = size / 4;

        FILE *fp = fopen(argv[1], "r"); /* open file */
        assert(fp != NULL);

        UArray_T segment0 = setup_seg0(fp, num_words); /* set up segment 0 */
        run_program(segment0); /* run command loop */
        fclose(fp); /* close file */

        return EXIT_SUCCESS;
}