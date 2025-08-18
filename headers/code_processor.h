/**
 * This is the machine code header file.
 * This file handles the conversion of instructions and operations into machine code,
 * managing memory usage and error handling.
 */
#ifndef CODE_PR0CESSOR_H
#define CODE_PR0CESSOR_H
#include "utils.h"

/**
 * Adds a data code to the data array.
 * Converts the given number to its 15-bit 2's complement binary representation and adds it to the array.
 * @data: Pointer to the array holding the data code.
 * @dc: Pointer to the data counter.
 * @number: The number to be added as data code.
 */
void add_data(unsigned short *data,int *dc,int number);


/**
 * Adds an instruction code to the code array.
 * Checks for memory limits before adding the code to the array.
 * @code: Pointer to the array holding the instruction code.
 * @usage: Pointer to the usage counter for memory.
 * @ic: Pointer to the instruction counter.
 * @word: The instruction code to be added.
 * @errors_found: Pointer to the error counter.
 */
void add_instruction(unsigned short *code,int *usage,int *ic,unsigned short word,int *errors_found);


/**
 * Processes and encodes the operation code for a given operand using bit-wise opertions.
 * Handles different addressing methods and updates the machine code accordingly.
 * @code: Pointer to the array holding the machine code.
 * @usage: Pointer to the usage counter for memory.
 * @ic: Pointer to the instruction counter.
 * @line: Pointer to the structure representing the current line of code being processed.
 * @method: The addressing method of the operand.
 * @operand: The operand string.
 * @operands_num: Number of operands in the instruction.
 * @errors_found: Pointer to the error counter.
 */
void process_operations(unsigned short *code,int *Usage,int *IC,Line *line,int method,char *operand,int operands_num,int *errors_found);


/**
 * Handles the encoding and processing of an instruction with one operand.
 * Generates machine code for the instruction and manages the different addressing methods for the operand.
 * @code: Pointer to the array holding the machine code.
 * @usage: Pointer to the usage counter for memory.
 * @ic: Pointer to the instruction counter.
 * @line: Pointer to the structure representing the current line of code being processed.
 * @method: The addressing method of the operand.
 * @operand: The operand string.
 * @ind: Index of the instruction in the opcode table.
 * @errors_found: Pointer to the error counter.
 */
void process_one_operand(unsigned short *code,int *Usage,int *IC,Line *line,int method,char *operand,int ind,int *errors_found);


/**
 * Handles the encoding and processing of an instruction with two operands.
 * Generates machine code for the instruction and manages the different addressing methods for the operands.
 * @code: Pointer to the array holding the machine code.
 * @usage: Pointer to the usage counter for memory.
 * @ic: Pointer to the instruction counter.
 * @line: Pointer to the structure representing the current line of code being processed.
 * @operand: The first operand string.
 * @second_operand: The second operand string.
 * @ind: Index of the instruction in the opcode table.
 * @errors_found: Pointer to the error counter.
 */
void process_two_operands(unsigned short *code,int *Usage,int *IC,Line *line,char *operand,char *second_operand,int ind,int *errors_found);


#endif
