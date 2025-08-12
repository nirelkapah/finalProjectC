# Compiler and flags
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Iheaders

# Executable target
assembler: assembler.o pre_processor.o macro_handler.o assembler_first_pass.o second_pass.o labels.o validator.o utils.o machine_code.o error_handler.o
	$(CC) $(CFLAGS) assembler.o pre_processor.o macro_handler.o assembler_first_pass.o second_pass.o labels.o validator.o utils.o machine_code.o error_handler.o -o assembler

# Object file rules
assembler.o: source/assembler.c headers/error_handler.h headers/validator.h headers/utils.h headers/macro_handler.h headers/labels.h headers/machine_code.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/assembler.c -o assembler.o

pre_processor.o: source/pre_processor.c headers/pre_processor.h headers/error_handler.h headers/validator.h headers/utils.h headers/macro_handler.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/pre_processor.c -o pre_processor.o

macro_handler.o: source/macro_handler.c headers/macro_handler.h headers/error_handler.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/macro_handler.c -o macro_handler.o

assembler_first_pass.o: source/assembler_first_pass.c headers/assembler_first_pass.h headers/validator.h headers/error_handler.h headers/macro_handler.h headers/labels.h headers/utils.h headers/second_pass.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/assembler_first_pass.c -o assembler_first_pass.o

second_pass.o: source/second_pass.c headers/second_pass.h headers/error_handler.h headers/validator.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/second_pass.c -o second_pass.o

labels.o: source/labels.c headers/labels.h headers/error_handler.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/labels.c -o labels.o

validator.o: source/validator.c headers/validator.h headers/error_handler.h headers/utils.h headers/macro_handler.h headers/labels.h headers/machine_code.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/validator.c -o validator.o

utils.o: source/utils.c headers/utils.h headers/error_handler.h headers/macro_handler.h headers/labels.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/utils.c -o utils.o

machine_code.o: source/machine_code.c headers/machine_code.h headers/error_handler.h headers/validator.h headers/labels.h headers/macro_handler.h headers/utils.h headers/definitions.h
	$(CC) $(CFLAGS) -c source/machine_code.c -o machine_code.o

error_handler.o: source/error_handler.c headers/error_handler.h
	$(CC) $(CFLAGS) -c source/error_handler.c -o error_handler.o

# Clean up object files and the executable
clean:
	rm -f *.o assembler

