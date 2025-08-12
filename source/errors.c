/* This file handles the printing of errors.*/
#include <stdio.h>
#include "errors.h"

/* The Following array specifies the error code numbers and the corresponding error message */
Error errors[] = {
        /* System errors */
        {Error_0, "No Files Entered"},
        {Error_1, "Memory Allocation Failed"},
        {Error_4, "Failed To Delete Redundant File"},
        {Error_5, "Failed To Open Existing File For Reading"},
        {Error_6, "Failed To Open New File For Writing"},
        {Error_73, "Memory Capacity Exceeded; Assembler Machine-Coding Suspended, Line Scanning Continues"},

        /* Syntax errors */
        {Error_2, "File Names Must Be Provided Without The .as Extension"},
        {Error_7, "Line Is Too Long — 80 Characters Maximum"},
        {Error_8, "Invalid Macro Declaration — No Name Defined"},
        {Error_9, "Invalid Macro Declaration — \"macr\" Command With Extraneous Text"},
        {Error_10, "Invalid Macro Declaration — Opcode/Instruction Name Cannot Be Used As A Macro Name"},
        {Error_11, "Invalid Macro Declaration — Register Name Cannot Be Used As A Macro Name"},
        {Error_13, "Invalid Macro Declaration — Macro Must Contain Exactly One Name"},
        {Error_14, "Invalid Macro Declaration — Macro Name Already In Use"},
        {Error_15, "Invalid Macro Declaration — \"endmacr\" Command With Extraneous Text"},
        {Error_17, "Invalid Macro Declaration — Empty Macro Content"},
        {Error_18, "Invalid Macro Declaration — \"macr\" Is A Reserved Word"},
        {Error_19, "Invalid Macro Declaration — \"endmacr\" Is A Reserved Word"},
        {Error_20, "Invalid Label Declaration — Label Name Is Too Long — 30 Characters Maximum"},
        {Error_21, "Invalid Label Declaration — Label Name Must Contain Alphabetic Or Numeric Characters Only"},
        {Error_22, "Invalid Label Declaration — No Value Associated With Label"},
        {Error_23, "Invalid Label Declaration — Label Name Cannot Be The Same As A Macro Name"},
        {Error_24, "Invalid Label Declaration — Opcode/Instruction Name Cannot Be Used As A Label Name"},
        {Error_25, "Invalid Label Declaration — Register Name Cannot Be Used As A Label Name"},
        {Error_27, "Invalid Label Declaration — Label Name Already In Use"},
        {Error_28, "Invalid Label Declaration — No Name Defined"},
        {Error_29, "Invalid Label Declaration — Label Name Must Start With An Alphabetic Character"},
        {Error_30, "Invalid Label Declaration — \"macr\" Is A Reserved Word"},
        {Error_31, "Invalid Label Declaration — \"endmacr\" Is A Reserved Word"},
        {Error_32, "Invalid Label Declaration — Local Label Name Cannot Be The Same As An External Label Name"},  /* "Local" means regular/entry label */
        {Error_33, "Macro Call Was Missed During Preprocessing — Macro Calls Are Allowed Once Per Line With No Extra Text"},
        {Error_34, "Instruction \".data\" Has An Illegal Comma Following The Instruction Name"},
        {Error_35, "Instruction \".data\" Has Illegal Characters — Only Integers Allowed"},
        {Error_36, "Instruction \".data\" Has A Missing Comma"},
        {Error_37, "Instruction \".data\" Has Multiple Consecutive Commas"},
        {Error_38, "Instruction \".data\" Expects An Integer After The Last Comma"},
        {Error_39, "Instruction \".data\" Has A Number That Is Out Of Range"},
        {Error_40, "Instruction \".data\" Has No Parameters"},
        {Error_41, "Instruction \".string\" Expects One String Parameter Enclosed In Double Quotes (\" \")"},
        {Error_42, "Instruction \".string\" Has No Parameter"},
        {Error_43, "Instructions \".entry\" And \".extern\" Must Have A Label Declaration"},
        {Error_44, "Instructions \".entry\" And \".extern\" Allow One Label Declaration At A Time"},
        {Error_45, "Instructions \".entry\" And \".extern\" Label Name Must Refer To A Valid Label"},
        {Error_46, "Instructions \".entry\" And \".extern\" Label Name Cannot Be The Same As A Macro Name"},
        {Error_47, "Instruction \".entry\" Label Name Cannot Be The Same As An External Label Name"},
        {Error_48, "Instruction \".extern\" Label Name Cannot Be The Same As A Local Label Name"},  /* "Local" means regular/entry label */
        {Error_49, "Operation Has Extraneous Text — No Operands Required"},
        {Error_50, "Operation Has A Missing Operand"},
        {Error_51, "Operation Has Missing Operands"},
        {Error_52, "Operation Has Extraneous Text — Only One Operand Is Required"},
        {Error_53, "Operation Has An Illegal Comma After The Operation Name"},
        {Error_54, "Operation Has Extraneous Text — Only Two Operands Are Required"},
        {Error_55, "Operation Has Multiple Consecutive Commas"},
        {Error_56, "Operation Has A Missing Comma"},
        {Error_57, "Operation Uses An Invalid Addressing Method For A Destination Operand"},
        {Error_58, "Operation Uses An Invalid Addressing Method For A Source Operand"},
        {Error_74, "Instruction \".mat\" Has Invalid Syntax — Expected Format: .mat [rows][cols] values..."},
        {Error_75, "Invalid Matrix Operand Syntax — Expected Format: LABEL[rX][rY]"},
        {Error_76, "Instruction \".mat\" Has More Values Than Allowed By rows * cols Definition"},
        {Error_59, "Operand Recognized As A Label But Is Not Valid — Check Syntax"},
        {Error_60, "Operand Uses The IMMEDIATE Addressing Method But Has No Value"},
        {Error_61, "Operand Invalid For The IMMEDIATE Addressing Method — Only Integers Allowed"},
        {Error_62, "Operand Out Of Range For The IMMEDIATE Addressing Method"},
        {Error_63, "Operand Uses The REGISTER Addressing Method But Has No Value"},
        {Error_64, "Operand Is Not A Valid Register For The REGISTER Addressing Method"},
        {Error_67, "Unrecognized Command — An Instruction Must Start With A Dot (.)"}, 
        {Error_68, "Unrecognized Command — Check Syntax"},
        {Error_69, "Unrecognized Operand — Check Syntax"},
        {Error_70, "Invalid Operand — Reserved Words And Macro Names Cannot Be Used As Operands"},
        {Error_72, "Label Was Declared As \".entry\" But Was Not Defined"},
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

void log_system_error(int error_code) {
        printf("ERROR (CODE_%d)  %s\n", error_code, find_error_msg(error_code));
}

void log_syntax_error(int error_code, char *file_name, int line_num) {
    printf("ERROR (CODE_%d)  File \"%s\" at line %d | %s\n", error_code, file_name, line_num, find_error_msg(error_code));
}

void log_unique_error(int error_code, char *file_name, int line_num, char *word) {
    /* Printing the file name, assembly line number, specific error quoted and the error message */
    printf("ERROR (CODE_%d)  File \"%s\" at line %d | \"%s\" | %s\n", error_code, file_name, line_num, word, find_error_msg(error_code));
}
