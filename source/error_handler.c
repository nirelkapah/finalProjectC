/* This file handles the printing of errors.*/
#include <stdio.h>
#include "error_handler.h"

/* The Following array specifies the error code numbers and the corresponding error message */
Error errors[] = {
        /* System errors */
        {Error_100, "No Files Entered"},
        {Error_101, "Memory Allocation Failed"},
        {Error_102, "Failed To Delete Redundant File"},
        {Error_103, "Failed To Open Existing File For Reading"},
        {Error_104, "Failed To Open New File For Writing"},
        {Error_105, "Memory Capacity Exceeded; Assembler Machine-Coding Suspended, Line Scanning Continues"},

        /* Syntax errors */
        {Error_200, "File Names Must Be Provided Without The .as Extension"},
        {Error_201, "Line Is Too Long — 80 Characters Maximum"},
        {Error_202, "Invalid Macro Declaration — No Name Defined"},
        {Error_203, "Invalid Macro Declaration — \"macr\" Command With Extraneous Text"},
        {Error_204, "Invalid Macro Declaration — Opcode/Instruction Name Cannot Be Used As A Macro Name"},
        {Error_205, "Invalid Macro Declaration — Register Name Cannot Be Used As A Macro Name"},
        {Error_206, "Invalid Macro Declaration — Macro Must Contain Exactly One Name"},
        {Error_207, "Invalid Macro Declaration — Macro Name Already In Use"},
        {Error_208, "Invalid Macro Declaration — \"endmacr\" Command With Extraneous Text"},
        {Error_209, "Invalid Macro Declaration — Empty Macro Content"},
        {Error_210, "Invalid Macro Declaration — \"macr\" Is A Reserved Word"},
        {Error_211, "Invalid Macro Declaration — \"endmacr\" Is A Reserved Word"},
        {Error_212, "Invalid Label Declaration — Label Name Is Too Long — 30 Characters Maximum"},
        {Error_213, "Invalid Label Declaration — Label Name Must Contain Alphabetic Or Numeric Characters Only"},
        {Error_214, "Invalid Label Declaration — No Value Associated With Label"},
        {Error_215, "Invalid Label Declaration — Label Name Cannot Be The Same As A Macro Name"},
        {Error_216, "Invalid Label Declaration — Opcode/Instruction Name Cannot Be Used As A Label Name"},
        {Error_217, "Invalid Label Declaration — Register Name Cannot Be Used As A Label Name"},
        {Error_218, "Invalid Label Declaration — Label Name Already In Use"},
        {Error_219, "Invalid Label Declaration — No Name Defined"},
        {Error_220, "Invalid Label Declaration — Label Name Must Start With An Alphabetic Character"},
        {Error_221, "Invalid Label Declaration — \"macr\" Is A Reserved Word"},
        {Error_222, "Invalid Label Declaration — \"endmacr\" Is A Reserved Word"},
        {Error_223, "Invalid Label Declaration — Local Label Name Cannot Be The Same As An External Label Name"},  /* "Local" means regular/entry label */
        {Error_224, "Macro Call Was Missed During Preprocessing — Macro Calls Are Allowed Once Per Line With No Extra Text"},
        {Error_225, "Instruction \".data\" Has An Illegal Comma Following The Instruction Name"},
        {Error_226, "Instruction \".data\" Has Illegal Characters — Only Integers Allowed"},
        {Error_227, "Instruction \".data\" Has A Missing Comma"},
        {Error_228, "Instruction \".data\" Has Multiple Consecutive Commas"},
        {Error_229, "Instruction \".data\" Expects An Integer After The Last Comma"},
        {Error_230, "Instruction \".data\" Has A Number That Is Out Of Range"},
        {Error_231, "Instruction \".data\" Has No Parameters"},
        {Error_232, "Instruction \".string\" Expects One String Parameter Enclosed In Double Quotes (\" \")"},
        {Error_233, "Instruction \".string\" Has No Parameter"},
        {Error_234, "Instructions \".entry\" And \".extern\" Must Have A Label Declaration"},
        {Error_235, "Instructions \".entry\" And \".extern\" Allow One Label Declaration At A Time"},
        {Error_236, "Instructions \".entry\" And \".extern\" Label Name Must Refer To A Valid Label"},
        {Error_237, "Instructions \".entry\" And \".extern\" Label Name Cannot Be The Same As A Macro Name"},
        {Error_238, "Instruction \".entry\" Label Name Cannot Be The Same As An External Label Name"},
        {Error_239, "Instruction \".extern\" Label Name Cannot Be The Same As A Local Label Name"},  /* "Local" means regular/entry label */
        {Error_240, "Operation Has Extraneous Text — No Operands Required"},
        {Error_241, "Operation Has A Missing Operand"},
        {Error_242, "Operation Has Missing Operands"},
        {Error_243, "Operation Has Extraneous Text — Only One Operand Is Required"},
        {Error_244, "Operation Has An Illegal Comma After The Operation Name"},
        {Error_245, "Operation Has Extraneous Text — Only Two Operands Are Required"},
        {Error_246, "Operation Has Multiple Consecutive Commas"},
        {Error_247, "Operation Has A Missing Comma"},
        {Error_248, "Operation Uses An Invalid Addressing Method For A Destination Operand"},
        {Error_249, "Operation Uses An Invalid Addressing Method For A Source Operand"},
        {Error_250, " \".mat\" Has Invalid Syntax — Expected Format: .mat [rows][cols] values..."},
        {Error_251, "Invalid Matrix Operand Syntax — Expected Format: LABEL[rX][rY]"},
        {Error_252, " \".mat\" Has More Values Than Allowed By rows * cols Definition"},
        {Error_253, "Operand Recognized As A Label But Is Not Valid — Check Syntax"},
        {Error_254, "Operand Uses The IMMEDIATE Addressing Method But Has No Value"},
        {Error_255, "Operand Invalid For The IMMEDIATE Addressing Method — Only Integers Allowed"},
        {Error_256, "Operand Out Of Range For The IMMEDIATE Addressing Method"},
        {Error_257, "Operand Uses The REGISTER Addressing Method But Has No Value"},
        {Error_258, "Operand Is Not A Valid Register For The REGISTER Addressing Method"},
        {Error_259, "Unrecognized Command — An Instruction Must Start With A Dot (.)"}, 
        {Error_260, "Unrecognized Command — Check Syntax"},
        {Error_261, "Unrecognized Operand — Check Syntax"},
        {Error_262, "Invalid Operand — Reserved Words And Macro Names Cannot Be Used As Operands"},
        {Error_263, "Label Was Declared As \".entry\" But Was Not Defined"},
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
