# Makefile for um 
# Alyssa Williams (awilli36) and Olivia Byun (obyun01)
# Last modified: 11/21/22
# 
# Includes build rules for um
#

############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii -lum-dis

# Compile flags
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64 -lcii

# Libraries needed for linking
LDLIBS = -lbitpack -lnetpbm -lcii40 -lm -lrt 

# Collect all .h files in our directory
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: um writetests

## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

# um:
um: um.o calculate.o memory.o perform_io.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)


clean:
	rm -f um *.o