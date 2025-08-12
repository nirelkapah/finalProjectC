/* This file handles the printing of errors.*/
#include <stdio.h>
#include "errors.h"

/* The Following array specifies the error code numbers and the corresponding error message */
Error errors[] = {
        /* System errors */
        {Error_0, "No files entered"},
        {Error_1, "Memory allocation failed"},
        {Error_4, "Failed to delete redundant file"},
        {Error_5, "Failed to open existing file for reading"},
        {Error_6, "Failed to open new file for writing"},
        {Error_73, "Memory capacity exceeded! Assembler machine-coding is suspended; line scanning continues"},

        /* Syntax errors */
        {Error_2, "File names must be provided without the .as extension"},
        {Error_7, "The line is too long, 80 characters max"},
        {Error_8, "Invalid macro declaration, no name was defined"},
        {Error_9, "Invalid macro declaration, \"macr\" command with extraneous text"},
        {Error_10, "Invalid macro declaration, an opcode / instruction name cannot be used as a macro name"},
        {Error_11, "Invalid macro declaration, a register name cannot be used as a macro name"},
        {Error_13, "Invalid macro declaration, macro should be assigned with one name"},
        {Error_14, "Invalid macro declaration, this macro name is already in use"},
        {Error_15, "Invalid macro declaration, \"endmacr\" command with extraneous text"},
        {Error_17, "Invalid macro declaration, empty macro content"},
        {Error_18, "Invalid macro declaration, \"macr\" is a reserved word"},
        {Error_19, "Invalid macro declaration, \"endmacr\" is a reserved word"},
        {Error_20, "Invalid label declaration, label name is too long, 30 characters max"},
        {Error_21, "Invalid label declaration, label name must contain alphabetic or numeric characters only"},
        {Error_22, "Invalid label declaration, no value associated with label"},
        {Error_23, "Invalid label declaration, a label name cannot be the same as a macro name"},
        {Error_24, "Invalid label declaration, an opcode / instruction name cannot be used as a label name"},
        {Error_25, "Invalid label declaration, a register name cannot be used as a label name"},
        {Error_27, "Invalid label declaration, this label name is already in use"},
        {Error_28, "Invalid label declaration, no name was defined"},
        {Error_29, "Invalid label declaration, label name must start with an alphabetic character"},
        {Error_30, "Invalid label declaration, \"macr\" is a reserved word"},
        {Error_31, "Invalid label declaration, \"endmacr\" is a reserved word"},
        {Error_32, "Invalid label declaration, local label name cannot be the same as an external label name"},  /* "Local" means regular/entry label */
        {Error_33, "Macro call was missed during preprocessing; macro calls are allowed once per line with no extra text"},
        {Error_34, "Instruction \".data\" has an illegal comma following the instruction name"},
        {Error_35, "Instruction \".data\" has illegal characters, only integers allowed"},
        {Error_36, "Instruction \".data\" has a missing comma"},
        {Error_37, "Instruction \".data\" has multiple consecutive commas"},
        {Error_38, "Instruction \".data\" expects an integer after the last comma"},
        {Error_39, "Instruction \".data\" has a number that is out of range"},
        {Error_40, "Instruction \".data\" has no parameters"},
        {Error_41, "Instruction \".string\" expects one string parameter enclosed in double quotes (\" \")"},
        {Error_42, "Instruction \".string\" has no parameter"},
        {Error_43, "Instructions \".entry\" and \".extern\" must have a label declaration"},
        {Error_44, "Instructions \".entry\" and \".extern\" allow one label declaration at a time"},
        {Error_45, "Instructions \".entry\" and \".extern\" label name must refer to a valid label"},
        {Error_46, "Instructions \".entry\" and \".extern\" label name cannot be the same as a macro name"},
        {Error_47, "Instruction \".entry\" label name cannot be the same as an external label name"},
        {Error_48, "Instruction \".extern\" label name cannot be the same as a local label name"},  /* "Local" means regular/entry label */
        {Error_49, "Operation has extraneous text, no operands required"},
        {Error_50, "Operation has a missing operand"},
        {Error_51, "Operation has missing operands"},
        {Error_52, "Operation has extraneous text, only one operand is required"},
        {Error_53, "Operation has an illegal comma after the operation name"},
        {Error_54, "Operation has extraneous text, only two operands are required"},
        {Error_55, "Operation has multiple consecutive commas"},
        {Error_56, "Operation has a missing comma"},
        {Error_57, "Operation uses an invalid addressing method for a destination operand"},
        {Error_58, "Operation uses an invalid addressing method for a source operand"},
        {Error_74, "Instruction \".mat\" has invalid syntax. Expected format: .mat [rows][cols] values..."},
        {Error_75, "Invalid matrix operand syntax. Expected format: LABEL[rX][rY]"},
        {Error_76, "Instruction \".mat\" has more values than allowed by rows * cols definition"},

        /* Specific (token-related) errors */
        {Error_59, "This operand was recognized as a label but is not valid, please check syntax"},
        {Error_60, "Operation has an operand that uses the IMMEDIATE addressing method but has no value"},
        {Error_61, "Operand is invalid for the IMMEDIATE addressing method, only integers allowed"},
        {Error_62, "Operand is out of range for the IMMEDIATE addressing method"},
        {Error_63, "Operation has an operand that uses the REGISTER addressing method but has no value"},
        {Error_64, "Operand is not a valid register for the REGISTER addressing method"},
        {Error_67, "Unrecognized command; an instruction must start with a dot (.)"}, 
        {Error_68, "Unrecognized command, please check syntax"},
        {Error_69, "Unrecognized operand, please check syntax"},
        {Error_70, "Invalid operand; reserved words and macro names cannot be used as operands"},
        {Error_72, " was declared as \".entry\" but was not defined"},
};

/* Lookup helper to find an error message by code, independent of array order */
static const char* find_error_msg(int error_code) {
        size_t i;
        size_t n = sizeof(errors) / sizeof(errors[0]);
        for (i = 0; i < n; ++i) {
                if (errors[i].code_num == error_code) return errors[i].error_msg;
        }
        return "Unknown error code";
}

void print_system_error(int error_code) {
        printf("ERROR (CODE_%d)  %s\n", error_code, find_error_msg(error_code));
}

void print_syntax_error(int error_code, char *file_name, int line_num) {
    printf("ERROR (CODE_%d)  File \"%s\" at line %d | %s\n", error_code, file_name, line_num, find_error_msg(error_code));
}

void print_specific_error(int error_code, char *file_name, int line_num, char *word) {
    /* Printing the file name, assembly line number, specific error quoted and the error message */
    printf("ERROR (CODE_%d)  File \"%s\" at line %d | \"%s\" | %s\n", error_code, file_name, line_num, word, find_error_msg(error_code));
}
