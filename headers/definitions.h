/**
 * This is the definitions header file.
 * This file contains all the constants and macros used in the program.
 */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* File and line constraints */
#define FILE_EXTENSION_LENGTH 3
#define MAX_SOURCE_LINE_LENGTH 81
#define MAX_MACRO_NAME_LENGTH 31
#define MAX_LABEL_NAME_LENGTH 31

/* Counts and capacities */
#define TOTAL_OPCODES 16
#define TOTAL_REGISTERS 8
#define TOTAL_INSTRUCTION_TYPES 5
#define MAX_ARRAY_CAPACITY 256

/* Address and numeric constants */
#define MEMORY_START_ADDRESS 100
#define BASE_10 10
#define BINARY_BASE 2

/* Bit manipulation and masks */
#define SINGLE_BIT_MASK 1
#define MASK_10_BITS 0x3FF
#define MASK_8_BITS 0xFF
#define MASK_4_BITS 0xF

/* Bit shift positions for instruction encoding */
#define OPCODE_SHIFT_POSITION 6
#define SOURCE_OPERAND_SHIFT_POSITION 4
#define DESTINATION_OPERAND_SHIFT_POSITION 2
#define IMMEDIATE_VALUE_SHIFT_POSITION 2
#define SOURCE_REGISTER_SHIFT_POSITION 6
#define DESTINATION_REGISTER_SHIFT_POSITION 2

/* Address Relocation Encoding (ARE) values */
#define ARE_EXTERNAL 1
#define ARE_RELOCATABLE 2
#define ARE_PLACEHOLDER_SIGNAL 3
#define ARE_ABSOLUTE 0

/* Numeric range limits */
#define MAX_10_BIT_SIGNED_VALUE 511
#define MIN_10_BIT_SIGNED_VALUE (-512)
#define MAX_REGISTER_NUMBER 7
#define MIN_REGISTER_NUMBER 0

/* Buffer and data constraints */
#define INTEGER_STRING_BUFFER_SIZE 20
#define MAX_DATA_VALUES_PER_LINE 38

/* Macro processing */
#define MACRO_START_LENGTH 4
#define MACRO_END_LENGTH 7

/* Matrix encoding */
#define MATRIX_ROW_REGISTER_SHIFT 6
#define MATRIX_COLUMN_REGISTER_SHIFT 2

/* Character constants */
#define PERIOD '.'
#define POUND_SIGN '#'
#define PLUS_SIGN '+'
#define MINUS_SIGN '-'
#define COLON_SIGN ':'
#define COMMA_SIGN ','
#define SEMICOLON ';'
#define ASTERISK_SIGN '*'
#define QUOTATION_MARK '\"'
#define STRING_TERMINATOR '\0'

#endif
