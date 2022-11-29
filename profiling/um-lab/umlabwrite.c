#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

extern void build_halt_test(Seq_T instructions);
extern void build_verbose_halt_test(Seq_T instructions);
extern void build_add_test(Seq_T instructions);
extern void build_add_test2(Seq_T stream);
extern void build_multiply_test(Seq_T stream);
extern void build_divide_test(Seq_T stream);
extern void build_nand_test(Seq_T stream);
extern void build_cmov_test(Seq_T stream);
extern void build_input_test(Seq_T stream);
void build_activate_test(Seq_T stream);
void build_inactivate_test(Seq_T stream);
void build_loadstore_test(Seq_T stream);
void build_loadstore2_test(Seq_T stream);
void build_loadp_test(Seq_T stream);
void build_loadp2_test(Seq_T stream);
void build_load_test(Seq_T stream);
void build_loop_test(Seq_T stream);
void build_loadp3_test(Seq_T stream);


/* The array `tests` contains all unit tests. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "halt",         NULL, "", build_halt_test },
        { "halt-verbose", NULL, "", build_verbose_halt_test },
        { "add", NULL, "", build_add_test },
        { "print-six", NULL, "6", build_add_test2},
        { "load-test", NULL, "dN[", build_load_test},
        { "multiply-test", NULL, "B!", build_multiply_test},
        { "divide-test", NULL, "<x", build_divide_test},
        { "nand-test", NULL, "FNv", build_nand_test},
        { "cmov-test", NULL, "MMlM", build_cmov_test},
        { "input-test", "l", "l", build_input_test},
        { "map-segment-test", NULL, "AB", build_activate_test},
        { "unmap-segment-test", NULL, "A", build_inactivate_test},
        { "loadstore-test", NULL, "C", build_loadstore_test},
        { "loadstore2-test", NULL, "C", build_loadstore2_test},
        { "loadp-test", NULL, "D", build_loadp_test},
        { "loadp2-test", NULL, "D", build_loadp2_test},
        { "loadp3-test", NULL, "F", build_loadp3_test},
        { "loop-test", NULL, "", build_loop_test}
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
