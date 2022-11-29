/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */
/*
        C: lsb = 0, width = 3
        B: lsb = 3, width = 3
        A: lsb = 6, width = 3
        trash: lsb = 4, width = 24
        OP: lsb = 28, width = 4
*/

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        uint64_t instruction = 0;

        /* set OP, A, B, and C */
        instruction = Bitpack_newu(instruction, 4, 28, op);
        instruction = Bitpack_newu(instruction, 3, 6, ra);
        instruction = Bitpack_newu(instruction, 3, 3, rb);
        instruction = Bitpack_newu(instruction, 3, 0, rc);
        
        return instruction;
}

/*
        value: lsb = 0, width = 25
        A: lsb = 25, width = 3
        OP: lsb = 28, width = 4
*/
Um_instruction loadval(unsigned ra, unsigned val)
{
        uint64_t instruction = 0;

        /* set A and value */
        instruction = Bitpack_newu(instruction, 4, 28, LV);
        instruction = Bitpack_newu(instruction, 3, 25, ra);
        instruction = Bitpack_newu(instruction, 25, 0, val);

        return instruction;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_test(Seq_T stream)
{
        /* load two values to add */
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_add_test2(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_mega_test(Seq_T stream)
{
        /*
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
        */
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 1));
        append(stream, loadval(r3, 1));

        append(stream, three_register(CMOV, r1, r2, r3));
        append(stream, three_register(SLOAD, r1, r2, r3));
        append(stream, add(r1, r2, r3));
        append(stream, three_register(MUL, r1, r2, r3));
        append(stream, three_register(DIV, r1, r2, r3));
        append(stream, three_register(NAND, r1, r2, r3));
        append(stream, three_register(ACTIVATE, r1, r2, r3));
        append(stream, three_register(INACTIVATE, r1, r2, r3));
        append(stream, three_register(OUT, r1, r2, r3));
        append(stream, three_register(IN, r1, r2, r3));
        append(stream, three_register(LOADP, r1, r2, r3));
        append(stream, loadval(r1, 0));
        append(stream, halt());
}

void build_load_test(Seq_T stream)
{
        append(stream, loadval(r7, 100));
        append(stream, loadval(r6, 78));
        append(stream, loadval(r0, 91));
        append(stream, output(r7));
        append(stream, output(r6));
        append(stream, output(r0));
        append(stream, halt());
}

void build_multiply_test(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r3, 33));
        // ! * 2 = B
        // printf("r1: %u r2: %u r3: %u\n", r1, r2, r3);

        append(stream, three_register(MUL, r1, r2, r3));
        append(stream, output(r1));
        append(stream, output(r3));
        append(stream, halt());
}

void build_divide_test(Seq_T stream)
{
        append(stream, loadval(r1, 81));
        append(stream, loadval(r2, 120));
        append(stream, loadval(r3, 2));
        
        append(stream, three_register(DIV, r1, r2, r3));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
        append(stream, loadval(r1, 104));
        append(stream, loadval(r2, 78));
        append(stream, loadval(r3, 118));

        append(stream, three_register(NAND, r1, r2, r3));
        append(stream, three_register(NAND, r1, r1, r1));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, output(r3));

        append(stream, halt());
}

void build_cmov_test(Seq_T stream)
{
        append(stream, loadval(r1, 108));
        append(stream, loadval(r2, 77));
        append(stream, loadval(r3, 1));

        /* test when register is NOT zero */
        append(stream, three_register(CMOV, r1, r2, r3));
        append(stream, output(r1));
        append(stream, output(r2));

        append(stream, loadval(r1, 108));
        append(stream, loadval(r2, 77));
        append(stream, loadval(r3, 0));
        
        /* test when register is zero */
        append(stream, three_register(CMOV, r1, r2, r3));
        append(stream, output(r1));
        append(stream, output(r2));

        append(stream, halt());
}

void build_input_test(Seq_T stream)
{
        append(stream, three_register(IN, r2, r3, r1));
        append(stream, output(r1));
        append(stream, halt());
}

void build_activate_test(Seq_T stream)
{
        append(stream, loadval(r3, 5));
        append(stream, three_register(ACTIVATE, r1, r2, r3));
        append(stream, loadval(r3, 64));
        append(stream, three_register(ADD, r1, r2, r3));
        append(stream, output(r1));

        append(stream, three_register(ACTIVATE, r1, r2, r3));
        append(stream, three_register(ADD, r1, r2, r3));
        append(stream, output(r1));

        append(stream, halt());
}

void build_inactivate_test(Seq_T stream)
{
        append(stream, loadval(r3, 5));
        append(stream, three_register(ACTIVATE, r1, r2, r3));
        append(stream, three_register(ACTIVATE, r1, r2, r3));
        append(stream, loadval(r2, 1));
        append(stream, three_register(INACTIVATE, r1, r3, r2));
        append(stream, three_register(ACTIVATE, r1, r1, r3));
        append(stream, loadval(r3, 64));
        append(stream, three_register(ADD, r1, r1, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void build_loadstore_test(Seq_T stream)
{
        append(stream, loadval(r3, 5));
        append(stream, three_register(ACTIVATE, r1, r2, r3));
        append(stream, loadval(r3, 67));
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 3));
        append(stream, three_register(SSTORE, r1, r2, r3));
        append(stream, three_register(SLOAD, r4, r1, r2));
        append(stream, output(r4));
        append(stream, halt());
}

void build_loadstore2_test(Seq_T stream)
{
        append(stream, loadval(r2, 8));
        append(stream, loadval(r3, 11));
        append(stream, loadval(r4, 0));
        append(stream, three_register(SLOAD, r5, r4, r2));
        append(stream, three_register(SLOAD, r6, r4, r3));
        append(stream, three_register(SSTORE, r4, r2, r6));
        append(stream, three_register(SSTORE, r4, r3, r5));

        append(stream, loadval(r0, 65));
        append(stream, halt());
        append(stream, three_register(ADD, r0, r1, r0));
        append(stream, output(r0));
        append(stream, loadval(r1, 2));
}

void build_loadp_test(Seq_T stream)
{
        append(stream, loadval(r2, 0));
        append(stream, loadval(r1, 68));
        append(stream, loadval(r3, 69));
        append(stream, output(r1));
        append(stream, loadval(r4, 7));
        append(stream, three_register(LOADP, r1, r2, r4));
        append(stream, output(r3));
        append(stream, halt());
}

void build_loadp2_test(Seq_T stream)
{
        append(stream, loadval(r2, 0));
        append(stream, loadval(r1, 68));
        append(stream, loadval(r3, 69));
        append(stream, output(r1));
        append(stream, loadval(r4, 7));
        append(stream, loadval(r5, 16384));
        append(stream, three_register(MUL, r6, r4, r5));
        append(stream, three_register(MUL, r6, r6, r5)); // now we have halt instruction

        append(stream, loadval(r7, 1));
        append(stream, three_register(ACTIVATE, r1, r2, r7));
        append(stream, loadval(r7, 0));
        append(stream, three_register(SSTORE, r2, r7, r6));

        append(stream, three_register(LOADP, r2, r2, r7));

        append(stream, output(r3));
}

void build_loadp3_test(Seq_T stream)
{
        append(stream, loadval(r0, 5));
        append(stream, loadval(r7, 65)); // A
        append(stream, three_register(ACTIVATE, r1, r1, r0)); // r1 has seg_id
        // 3657433092 = 4363 * 838284 + 1 load in 5 into register 5
        append(stream, loadval(r5, 4363));
        append(stream, loadval(r6, 838284));
        append(stream, three_register(MUL, r5, r5, r6));
        append(stream, loadval(r6, 1));
        append(stream, three_register(ADD, r5, r5, r6));
        append(stream, three_register(SSTORE, r1, r6, r5));
        
        // 805306735 = 720955 * 1117
        append(stream, loadval(r2, 720955));
        append(stream, loadval(r3, 1117));
        append(stream, loadval(r4, 2));
        append(stream, three_register(MUL, r5, r2, r3));
        append(stream, three_register(SSTORE, r1, r4, r5));

        // 2684354565 = 885 * 3033169
        append(stream, loadval(r2, 885));
        append(stream, loadval(r3, 3033169));
        append(stream, loadval(r4, 3));
        append(stream, three_register(MUL, r5, r2, r3));
        append(stream, three_register(SSTORE, r1, r4, r5));
        
        // load halt instruction
        append(stream, loadval(r4, 7));
        append(stream, loadval(r5, 16384));
        append(stream, three_register(MUL, r6, r4, r5));
        append(stream, three_register(MUL, r6, r6, r5)); // now we have halt instruction    
        append(stream, loadval(r4, 4));
        append(stream, three_register(SSTORE, r1, r4, r6));

        // load program 
        append(stream, loadval(r6, 1));
        append(stream, three_register(LOADP, r6, r6, r6));

}

void build_loop_test(Seq_T stream)
{
        append(stream, loadval(r0, 0));      // 0
        append(stream, loadval(r1, 500000)); // 1
        append(stream, loadval(r3, 5));        // 2
        append(stream, loadval(r6, 1));      // 3
        append(stream, loadval(r7, 68)); //68 = D

        
        append(stream, three_register(ADD, r0, r0, r6));   // 4
        append(stream, output(r7));
        append(stream, three_register(NAND, r2, r0, r0));  // 5
        append(stream, three_register(ADD, r2, r2, r1));   // 6
        append(stream, loadval(r4, 12));       // 7
        append(stream, three_register(CMOV, r4, r3, r2));  //if r[c] != 0 r[a] = r[b]
        append(stream, three_register(LOADP, r5, r5, r4)); // 9
        append(stream, halt());                            //10
}