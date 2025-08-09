# Compiler and flags
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Iheaders

# Executable target
assembler: assembler.o pre_proc.o macros.o first_pass.o second_pass.o labels.o analyzer.o utility.o machine_code.o errors.o
	$(CC) $(CFLAGS) assembler.o pre_proc.o macros.o first_pass.o second_pass.o labels.o analyzer.o utility.o machine_code.o errors.o -o assembler

# Object file rules
assembler.o: source/assembler.c headers/errors.h headers/analyzer.h headers/utility.h headers/macros.h headers/labels.h headers/machine_code.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/assembler.c -o assembler.o

pre_proc.o: source/pre_proc.c headers/pre_proc.h headers/errors.h headers/analyzer.h headers/utility.h headers/macros.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/pre_proc.c -o pre_proc.o

macros.o: source/macros.c headers/macros.h headers/errors.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/macros.c -o macros.o

first_pass.o: source/first_pass.c headers/first_pass.h headers/analyzer.h headers/errors.h headers/macros.h headers/labels.h headers/utility.h headers/second_pass.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/first_pass.c -o first_pass.o

second_pass.o: source/second_pass.c headers/second_pass.h headers/errors.h headers/analyzer.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/second_pass.c -o second_pass.o

labels.o: source/labels.c headers/labels.h headers/errors.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/labels.c -o labels.o

analyzer.o: source/analyzer.c headers/analyzer.h headers/errors.h headers/utility.h headers/macros.h headers/labels.h headers/machine_code.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/analyzer.c -o analyzer.o

utility.o: source/utility.c headers/utility.h headers/errors.h headers/macros.h headers/labels.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/utility.c -o utility.o

machine_code.o: source/machine_code.c headers/machine_code.h headers/errors.h headers/analyzer.h headers/labels.h headers/macros.h headers/utility.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/machine_code.c -o machine_code.o

errors.o: source/errors.c headers/errors.h
	$(CC) $(CFLAGS) -c source/errors.c -o errors.o

# Clean up object files and the executable
clean:
	rm -f *.o assembler

