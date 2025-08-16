/**
 * validator.h - Assembly Language Parser & Code Generator
 *
 * This module provides comprehensive parsing and analysis functionality for assembly code.
 * It handles label validation, instruction parsing, and machine code generation
 * with complete error checking and validation.
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H
#include "utils.h"

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
 * source_file: The source file being processed (for error reporting)
 * macro_identifier: The macro name to validate
 * line_number: The line number where this macro is defined
 * return 0 if valid, 1 if validation fails
 */
int validate_macro_identifier(char *source_file, char *macro_identifier, int line_number);

/**
 * Validate label identifier according to assembly language rules.
 * Performs comprehensive validation including length constraints, character validation,
 * reserved word conflicts, and type-specific requirements.
 * 
 * label_identifier: The label name to validate
 * label_type: The label classification (REGULAR, ENTRY, EXTERN, etc.)
 * context: Line context information for error reporting
 * error_counter: Error counter for tracking validation failures
 * return 0 if valid, 1 if invalid
 */
int validate_label_identifier(char *label_identifier, Type label_type, Line *context, int *error_counter);


/**
 * Look up instruction by mnemonic name.
 * Searches the instruction table for the specified mnemonic and returns
 * its index position. Returns -1 if the instruction is not supported.
 * 
 * token: The instruction mnemonic to search for
 * return Index in instruction table, or -1 if not found
 */
int lookup_instruction_opcode(char *token);

/**
 * Parse and validate register operand.
 * Determines if the given token represents a valid register (r0-r7)
 * and returns the corresponding register number.
 * 
 * register_token: The token to validate as a register
 * return Register number (0-7) or -1 if invalid
 */
int parse_register_operand(char *register_token);

/**
 * Identify assembler directive type.
 * Determines if the given token represents a valid assembler directive
 * such as .data, .string, .entry, .extern, or .mat.
 * 
 * directive_token: The token to analyze for directive classification
 * return Directive type index, or -1 if not a directive
 */
int identify_assembler_directive(char *directive_token);


/**
 * Determine operand addressing mode.
 * Analyzes operand syntax to classify the addressing method:
 * immediate (#value), direct (label), matrix (label[reg][reg]), or register (rX).
 * 
 * operand_text: The operand string to analyze
 * context: Line context information for error reporting
 * error_counter: Error counter for tracking validation failures
 * return Addressing mode enum, or -1 if invalid
 */
int determine_operand_addressing_mode(char *operand_text, Line *context, int *error_counter);

/**
 * Check for reserved word conflicts in identifiers.
 * Validates that user-defined identifiers (labels, macros) do not conflict
 * with reserved words like instruction mnemonics, register names, or directives.
 * 
 * source_file: Source file name for error reporting
 * identifier: The identifier to validate
 * line_number: Line number for error context
 * identifier_type: The classification of this identifier
 * return 0 if name is valid, 1 if it conflicts with reserved words
 */
int check_reserved_word_conflict(char *source_file, char *identifier, int line_number, Type identifier_type);


/**
 * Handle directives like .data, .string, .entry, etc.
 * These don't generate executable code, but they do stuff like
 * define data or mark symbols for export.
 * 
 * data_segment: Where we store data values
 * memory_usage: How much memory we've used
 * data_counter: Position in data segment
 * context: Line info for errors
 * parse_position: Where we are in parsing
 * current_token: The directive we're processing
 * error_counter: Tracks errors
 * return 1 if we handled a directive, 0 if not
 */
int parse_assembler_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *parse_position, char *current_token, int *error_counter);

/**
 * Handle actual executable instructions like mov, add, etc.
 * These generate machine code that the CPU will execute.
 * 
 * instruction_segment: Where we store machine code
 * memory_usage: How much memory we've used
 * instruction_counter: Position in instruction segment
 * context: Line info for errors
 * parse_position: Where we are in parsing
 * current_token: The instruction we're processing
 * error_counter: Tracks errors
 * return 1 if we handled an instruction, 0 if not
 */
int parse_executable_instruction(unsigned short *instruction_segment, int *memory_usage, int *instruction_counter, Line *context, char *parse_position, char *current_token, int *error_counter);

/**
 * Check if an addressing mode is valid for a specific instruction operand.
 * Different instructions have different rules about which addressing modes
 * they can use for source vs destination operands.
 * 
 * context: Line context for error reporting
 * addressing_mode: The addressing mode to validate
 * instruction_index: Which instruction we're checking
 * operand_position: Whether this is source (0) or destination (1) operand
 * error_counter: Error tracking counter
 * return 0 if compatible, 1 if incompatible
 */
int validate_addressing_mode_compatibility(Line *context, int addressing_mode, int instruction_index, int operand_position, int *error_counter);

/**
 * Process .data directive for numeric data storage.
 * Parses comma-separated numeric values and stores them in the data segment
 * with appropriate range validation and memory management.
 * 
 * data_segment: Data segment array for numeric storage
 * memory_usage: Memory usage tracking counter
 * data_counter: Current position in data segment
 * context: Line context information for error reporting
 * value_list: Comma-separated list of numeric values
 * error_counter: Error counter for tracking validation failures
 */
void process_data_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *value_list, int *error_counter);

/**
 * Process .string directive for character data storage.
 * Parses string literals and stores individual characters in the data segment
 * with automatic null termination for proper string handling.
 * 
 * data_segment: Data segment array for character storage
 * memory_usage: Memory usage tracking counter
 * data_counter: Current position in data segment
 * context: Line context information for error reporting
 * string_literal: The quoted string literal to process
 * error_counter: Error counter for tracking validation failures
 */
void process_string_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *string_literal, int *error_counter);


/**
 * Process .entry directive for symbol export.
 * Designates labels as externally visible symbols for inter-module linking.
 * Entry labels are exported to the .ent output file for linker processing.
 * 
 * context: Line context information for error reporting
 * label_list: The labels to designate as entry points
 * error_counter: Error counter for tracking validation failures
 */
void process_entry_directive(Line *context, char *label_list, int *error_counter);

/**
 * Process .extern directive for external symbol declaration.
 * Declares symbols that are defined in other modules and will be
 * resolved during the linking phase of the assembly process.
 * 
 * context: Line context information for error reporting
 * symbol_list: The external symbols to declare
 * error_counter: Error counter for tracking validation failures
 */
void process_extern_directive(Line *context, char *symbol_list, int *error_counter);

/**
 * Process .mat directive for matrix data structure definition.
 * Handles two-dimensional array declarations with specified dimensions
 * and initializes matrix data in the data segment.
 * 
 * data_segment: Data segment array for matrix storage
 * memory_usage: Memory usage tracking counter
 * data_counter: Current position in data segment
 * context: Line context information for error reporting
 * matrix_definition: The matrix specification and initialization data
 * error_counter: Error counter for tracking validation failures
 */
void process_matrix_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *matrix_definition, int *error_counter);

/**
 * Generate machine code for assembly instructions.
 * Translates assembly language instructions into executable machine code
 * with comprehensive validation of operands, addressing modes, and syntax.
 * 
 * instruction_segment: Instruction segment array for machine code storage
 * memory_usage: Memory usage tracking counter
 * instruction_counter: Current position in instruction segment
 * context: Line context information for error reporting
 * operand_list: The instruction operand specification
 * instruction_index: Index of the instruction in the instruction table
 * error_counter: Error counter for tracking validation failures
 */
void generate_instruction_machine_code(unsigned short *instruction_segment, int *memory_usage, int *instruction_counter, Line *context, char *operand_list, int instruction_index, int *error_counter);

/**
 * Parse and encode numeric data from .data directives.
 * Processes comma-separated numeric values with range validation
 * and stores the encoded data in the appropriate data segment.
 * 
 * data_segment: Data segment array for encoded numeric storage
 * memory_usage: Memory usage tracking counter
 * data_counter: Current position in data segment
 * context: Line context information for error reporting
 * numeric_list: Comma-separated list of numeric values to parse
 * error_counter: Error counter for tracking validation failures
 */
void parse_and_encode_numeric_data(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *numeric_list, int *error_counter);

#endif
