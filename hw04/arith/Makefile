# Makefile for arith (Comp 40 Assignment 4)
# Helena Lowe (hlowe01) and Olivia Byun (obyun01)
# Last modified: 10/26/22
# 
# Includes build rules for ppmdiff and 40image
#

############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
LDLIBS = -l40locality -lnetpbm -lcii40 -lm -lrt -larith40

# Collect all .h files in our directory
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: ppmdiff 40image

## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

# ppmdiff:
ppmdiff: ppmdiff.o a2plain.o uarray2.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# 40image:
40image: compress40.o a2plain.o uarray2.o compress.o decompress.o ppm_rgb.o \
		40image.o transform.o bitpack.o codewords.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f ppmdiff 40image *.o