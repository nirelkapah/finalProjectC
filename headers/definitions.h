/**
 * This is the definitions header file.
 */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* File and line constraints */
#define EXTENSION_LEN 3
#define MAX_LINE_LENGTH 81
#define MAX_MACRO_LENGTH 31
#define MAX_LABEL_LENGTH 31

/* Counts and capacities */
#define OPCODES_COUNT 16
#define REGISTERS_COUNT 8
#define INSTRUCTIONS_COUNT 5
#define CAPACITY 256

/* Address and numeric constants */
#define STARTING_ADDRESS 100
#define DECIMAL_BASE 10
#define TWO 2

/* Bit manipulation and masks */
#define BIT_MASK 1
#define MASK_10BIT 0x3FF
#define MASK_8BIT 0xFF
#define MASK_4BIT 0xF

/* Bit shift positions */
#define SHIFT_OPCODE_POS 6
#define SHIFT_SRC_OPERAND 4
#define SHIFT_DST_OPERAND 2
#define SHIFT_IMMEDIATE_VALUE 2
#define SHIFT_SRC_REGISTER 6
#define SHIFT_DST_REGISTER 2

/* ARE encoding values */
#define BIT_MASK_EXTERNAL 1
#define BIT_MASK_RELOCATABLE 2
#define BIT_MASK_SIGNAL 3
#define BIT_MASK_ABSOLUTE 0

/* Numeric range limits */
#define MAX_10BIT 511
#define MIN_10BIT (-512)
#define MAX_REGISTER 7
#define MIN_REGISTER 0

/* Buffer and data constraints */
#define BUFFER_SIZE 20
#define MAX_DATA_NUM 38

/* Macro processing */
#define MACR_LENGTH 4
#define ENDMACR_LENGTH 7

/* Matrix encoding */
#define MATRIX_ROW_SHIFT 6
#define MATRIX_COL_SHIFT 2

/* Character constants */
#define DOT '.'
#define HASH '#'
#define PLUS '+'
#define MINUS '-'
#define COLON ':'
#define COMMA ','
#define COMMENT ';'
#define ASTERISK '*'
#define DOUBLE_QUOTE '\"'
#define NULL_TERMINATOR '\0'

#endif
