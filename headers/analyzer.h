/**
 * analyzer.h - Assembly Language Parser & Code Generator
 * 
 * This module provides comprehensive parsing and analysis functionality for assembly code.
 * It handles label validation, instruction parsing, and machine code generation
 * with complete error checking and validation.
 */

#ifndef ANALYZER_H
#define ANALYZER_H
#include "utility.h"

/* Which addressing modes each instruction can handle */
typedef enum SupportedAddressingModes {
    ALL_MODES,           
    NO_MODES,            
    DIRECT_ONLY,         
    DIRECT_AND_REGISTER, 
    ALL_EXCEPT_IMMEDIATE 
} SupportedAddressingModes;

/* Struct for an instruction */
typedef struct InstructionDefinition {
    char *mnemonic;                            
    int opcode_value;                           
    int operand_count;                         
    SupportedAddressingModes source_modes;     
    SupportedAddressingModes destination_modes; 
} InstructionDefinition;

/* The four ways you can reference data in our assembly language */
typedef enum Addressing_Method {
    IMMEDIATE = 0,        
    DIRECT = 1,          
    MATRIX = 2,          
    DIRECT_REGISTER = 3 
} Addressing_Method;

/**
 * Retrieve the instruction definition table.
 * Returns a pointer to the static array containing comprehensive information
 * about all supported instructions, including opcodes, operand counts, and addressing modes.
 */
InstructionDefinition *retrieve_instruction_set();

/**
 * Validate macro identifier naming conventions.
 * Ensures the macro name meets length requirements, uses valid characters,
 * and does not conflict with reserved words or system identifiers.
 * 
 * @param source_file The source file being processed (for error reporting)
 * @param macro_identifier The macro name to validate
 * @param line_number The line number where this macro is defined
 * @return 0 if valid, 1 if validation fails
 */
int validate_macro_identifier(char *source_file, char *macro_identifier, int line_number);

/**
 * Validate label identifier according to assembly language rules.
 * Performs comprehensive validation including length constraints, character validation,
 * reserved word conflicts, and type-specific requirements.
 * 
 * @param label_identifier The label name to validate
 * @param label_type The label classification (REGULAR, ENTRY, EXTERN, etc.)
 * @param context Line context information for error reporting
 * @param error_counter Error counter for tracking validation failures
 * @return 0 if valid, 1 if invalid
 */
int validate_label_identifier(char *label_identifier, Type label_type, Line *context, int *error_counter);


/**
 * Look up instruction by mnemonic name.
 * Searches the instruction table for the specified mnemonic and returns
 * its index position. Returns -1 if the instruction is not supported.
 * 
 * @param token The instruction mnemonic to search for
 * @return Index in instruction table, or -1 if not found
 */
int lookup_instruction_opcode(char *token);

/**
 * Parse and validate register operand.
 * Determines if the given token represents a valid register (r0-r7)
 * and returns the corresponding register number.
 * 
 * @param register_token The token to validate as a register
 * @return Register number (0-7) or -1 if invalid
 */
int parse_register_operand(char *register_token);

/**
 * Identify assembler directive type.
 * Determines if the given token represents a valid assembler directive
 * such as .data, .string, .entry, .extern, or .mat.
 * 
 * @param directive_token The token to analyze for directive classification
 * @return Directive type index, or -1 if not a directive
 */
int identify_assembler_directive(char *directive_token);


/**
 * Determine operand addressing mode.
 * Analyzes operand syntax to classify the addressing method:
 * immediate (#value), direct (label), matrix (label[reg][reg]), or register (rX).
 * 
 * @param operand_text The operand string to analyze
 * @param context Line context information for error reporting
 * @param error_counter Error counter for tracking validation failures
 * @return Addressing mode enum, or -1 if invalid
 */
int determine_operand_addressing_mode(char *operand_text, Line *context, int *error_counter);

/**
 * Validate identifier against reserved word conflicts.
 * Ensures that user-defined identifiers do not conflict with system reserved words,
 * instruction mnemonics, register names, or directive keywords.
 * 
 * @param source_file Source file name for error message context
 * @param identifier The identifier name to validate
 * @param line_number Line number for error reporting
 * @param identifier_type The classification of this identifier
 * @return 0 if name is valid, 1 if it conflicts with reserved words
 */
int check_reserved_word_conflict(char *source_file, char *identifier, int line_number, Type identifier_type);


/**
 * Handle directives like .data, .string, .entry, etc.
 * These don't generate executable code, but they do stuff like
 * define data or mark symbols for export.
 * 
 * @param data_segment Where we store data values
 * @param memory_usage Tracks how much memory we're using
 * @param data_counter Current position in data segment
 * @param context Line info for errors
 * @param parse_position Where we are in parsing the line
 * @param current_token The directive we're processing
 * @param error_counter Tracks errors
 * @return 1 if we handled a directive, 0 if not
 */
int parse_assembler_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *parse_position, char *current_token, int *error_counter);

/**
 * Handle actual instructions like mov, add, jmp.
 * This is where we turn assembly instructions into machine code
 * that the CPU can actually execute.
 * 
 * @param instruction_segment Where we store instruction machine code
 * @param memory_usage Tracks memory usage
 * @param instruction_counter Current position in instruction segment
 * @param context Line info for errors
 * @param parse_position Where we are in parsing
 * @param current_token The instruction we're processing
 * @param error_counter Tracks errors
 * @return 1 if we handled an instruction, 0 if not
 */
int parse_executable_instruction(unsigned short *instruction_segment, int *memory_usage, int *instruction_counter, Line *context, char *parse_position, char *current_token, int *error_counter);


/**
 * Validate addressing mode compatibility with instruction requirements.
 * Verifies that the specified addressing mode is permitted for the given
 * instruction and operand position according to the instruction set architecture.
 * 
 * @param context Line context information for error reporting
 * @param addressing_mode The addressing mode to validate
 * @param instruction_index The instruction index in the instruction table
 * @param operand_position Source (0) or destination (1) operand position
 * @param error_counter Error counter for tracking validation failures
 * @return 0 if compatible, 1 if incompatible
 */
int validate_addressing_mode_compatibility(Line *context, int addressing_mode, int instruction_index, int operand_position, int *error_counter);


/**
 * Process .data directive for numeric data storage.
 * Parses comma-separated numeric values and stores them in the data segment
 * with appropriate range validation and memory management.
 * 
 * @param data_segment Data segment array for numeric storage
 * @param memory_usage Memory usage tracking counter
 * @param data_counter Current position in data segment
 * @param context Line context information for error reporting
 * @param value_list Comma-separated list of numeric values
 * @param error_counter Error counter for tracking validation failures
 */
void process_data_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *value_list, int *error_counter);

/**
 * Process .string directive for character data storage.
 * Parses string literals and stores individual characters in the data segment
 * with automatic null termination for proper string handling.
 * 
 * @param data_segment Data segment array for character storage
 * @param memory_usage Memory usage tracking counter
 * @param data_counter Current position in data segment
 * @param context Line context information for error reporting
 * @param string_literal The quoted string literal to process
 * @param error_counter Error counter for tracking validation failures
 */
void process_string_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *string_literal, int *error_counter);


/**
 * Process .entry directive for symbol export.
 * Designates labels as externally visible symbols for inter-module linking.
 * Entry labels are exported to the .ent output file for linker processing.
 * 
 * @param context Line context information for error reporting
 * @param label_list The labels to designate as entry points
 * @param error_counter Error counter for tracking validation failures
 */
void process_entry_directive(Line *context, char *label_list, int *error_counter);

/**
 * Process .extern directive for external symbol declaration.
 * Declares symbols that are defined in other modules and will be
 * resolved during the linking phase of the assembly process.
 * 
 * @param context Line context information for error reporting
 * @param symbol_list The external symbols to declare
 * @param error_counter Error counter for tracking validation failures
 */
void process_extern_directive(Line *context, char *symbol_list, int *error_counter);

/**
 * Process .mat directive for matrix data structure definition.
 * Handles two-dimensional array declarations with specified dimensions
 * and initializes matrix data in the data segment.
 * 
 * @param data_segment Data segment array for matrix storage
 * @param memory_usage Memory usage tracking counter
 * @param data_counter Current position in data segment
 * @param context Line context information for error reporting
 * @param matrix_definition The matrix specification and initialization data
 * @param error_counter Error counter for tracking validation failures
 */
void process_matrix_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *matrix_definition, int *error_counter);

/**
 * Generate machine code for assembly instructions.
 * Translates assembly language instructions into executable machine code
 * with comprehensive validation of operands, addressing modes, and syntax.
 * 
 * @param instruction_segment Instruction segment array for machine code storage
 * @param memory_usage Memory usage tracking counter
 * @param instruction_counter Current position in instruction segment
 * @param context Line context information for error reporting
 * @param operand_list The instruction operand specification
 * @param instruction_index Index of the instruction in the instruction table
 * @param error_counter Error counter for tracking validation failures
 */
void generate_instruction_machine_code(unsigned short *instruction_segment, int *memory_usage, int *instruction_counter, Line *context, char *operand_list, int instruction_index, int *error_counter);

/**
 * Parse and encode numeric data from .data directives.
 * Processes comma-separated numeric values with range validation
 * and stores the encoded data in the appropriate data segment.
 * 
 * @param data_segment Data segment array for encoded numeric storage
 * @param memory_usage Memory usage tracking counter
 * @param data_counter Current position in data segment
 * @param context Line context information for error reporting
 * @param numeric_list Comma-separated list of numeric values to parse
 * @param error_counter Error counter for tracking validation failures
 */
void parse_and_encode_numeric_data(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *numeric_list, int *error_counter);

#endif
