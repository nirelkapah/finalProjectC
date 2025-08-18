/* This file handles the printing of errors.*/
#include <stdio.h>
#include "error_handler.h"

/* This array specifies the error code numbers and the corresponding error message */

Error errors[] = {

        /* System errors */
        {Error_100, "No input files were supplied"},
        {Error_101, "Allocation request failed"},
        {Error_102, "Could not remove temporary file"},
        {Error_103, "Unable to open existing file for read access"},
        {Error_104, "Unable to create output file for write access"},
        {Error_105, "Out of memory; continuing to scan lines"},

        /* Syntax errors */
        {Error_200, "Source names must be provided without the .as"},
        {Error_201, "Line exceeds 80 characters"},
        {Error_202, "Missing identifier"},
        {Error_203, "mcro directive has trailing text"},
        {Error_204, "Opcode name cannot be used as a macro identifier"},
        {Error_205, "Register name cannot be used as a macro identifier"},
        {Error_206, "Macro header must declare one identifier"},
        {Error_207, "Macro identifier already defined"},
        {Error_208, "endmcro directive has trailing text"},
        {Error_209, "Macro body is empty"},
        {Error_210, "mcro is reserved"},
        {Error_211, "endmcro is reserved"},
        {Error_212, "name exceeds 30 characters when defining a label"},
        {Error_213, "non-alphanumeric characters present when defining a label"},
        {Error_214, "referenced before assignment when using a label"},
        {Error_215, "conflicts with an existing macro name when defining a label"},
        {Error_216, "opcode identifier used as a label name when defining a label"},
        {Error_217, "register identifier used as a label name when defining a label"},
        {Error_218, "name already declared earlier when defining a label"},
        {Error_219, "Identifier missing"},
        {Error_220, "name does not start with a letter when defining a label"},
        {Error_221, "\"mcro\" used as a label name when defining a label"},
        {Error_222, "\"endmcro\" used as a label name when defining a label"},
        {Error_223, "local name collides with an external symbol when defining a label"},
        {Error_224, "Macro invocation skipped in preprocessing; only one call per line with no extra text"},
        {Error_225, "stray comma immediately following the directive token when using .data"},
        {Error_226, "non-numeric character found in operand list when using .data"},
        {Error_227, "missing separating comma between values when using .data"},
        {Error_228, "consecutive commas with no value between when using .data"},
        {Error_229, "expected an integer after the final comma when using .data"},
        {Error_230, "numeric literal outside the permitted range when using .data"},
        {Error_231, "no values were supplied when using .data"},
        {Error_232, "exactly one quoted sequence must be provided when using .string"},
        {Error_233, "missing argument when using .string"},
        {Error_234, "expecting a single label immediately after the directive when using .entry or .extern"},
        {Error_235, "too many operands provided when using .entry or .extern; supply exactly one label"},
        {Error_236, "label must reference an existing symbol when using .entry or .extern"},
        {Error_237, "macro identifiers are not permitted when using .entry or .extern"},
        {Error_238, "name collides with an external symbol when using .entry"},
        {Error_239, "name reuses a local symbol when using .extern"},
        {Error_240, "extraneous text present when assembling an instruction that takes no operands"},
        {Error_241, "no operand supplied when assembling an instruction that requires one"},
        {Error_242, "no operands supplied when assembling an instruction that requires two"},
        {Error_243, "additional text present when assembling an instruction that allows one operand"},
        {Error_244, "illegal comma immediately after the mnemonic when assembling an instruction"},
        {Error_245, "additional text present when assembling an instruction that requires exactly two operands"},
        {Error_246, "multiple consecutive commas in the operand list when assembling an instruction"},
        {Error_247, "missing comma between operands when assembling an instruction"},
        {Error_248, "invalid addressing mode for the destination operand when using this instruction"},
        {Error_249, "invalid addressing mode for the source operand when using this instruction"},
        {Error_250, "directive malformed when using .mat; expected: .mat [r][c]"},
        {Error_251, "invalid operand form when using a matrix operand; expected LABEL[r][c]"},
        {Error_252, "too many elements provided when using .mat; exceeds r*c"},
        {Error_253, "Operand looks like a label but is invalid; check spelling/syntax"},
        {Error_254, "no numeric value supplied when using immediate addressing"},
        {Error_255, "non-integer value supplied when using immediate addressing"},
        {Error_256, "numeric literal out of range when using immediate addressing"},
        {Error_257, "no register operand supplied when using register addressing"},
        {Error_258, "operand is not a valid register name when using register addressing"},
        {Error_259, "directive Unrecognized, commands must begin with a dot"}, 
        {Error_260, "instruction Unrecognized, verify syntax"},
        {Error_261, "operand Unrecognized, verify syntax"},
        {Error_262, "operand invalid, reserved words and macro names are not allowed"},
        {Error_263, "entry Symbol marked as .entry was never defined"},
};

static const char* look_up_error_message(int error_code) {
        size_t i;
        size_t n = sizeof(errors) / sizeof(errors[0]);
        for (i = 0; i < n; ++i) {
                if (errors[i].num == error_code) return errors[i].message;
        }
        return "Unknown error code";
}

void log_system_error(int error_code) {
        printf("ERROR (CODE_%d)  %s\n", error_code, look_up_error_message(error_code));
}

void log_syntax_error(int error_code, char *file_name, int line_num) {
    printf("ERROR (CODE_%d)  File \"%s\" at line %d | %s\n", error_code, file_name, line_num, look_up_error_message(error_code));
}

