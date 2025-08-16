/**
 * This is the largest file in the project, it handles the entire analysis of the assembly code.
 * It includes functions for validating labels, registers, operations and instructions,
 * as well as determining their types while handling all types of potential errors.
 * This phase ensures that the input assembly code is correctly formatted and valid
 * before proceeding to the code generation phase.
 * The file also contains definitions for opcodes, registers, and instructions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "error_handler.h"
#include "validator.h"
#include "utils.h"
#include "macro_handler.h"
#include "labels_handler.h"
#include "code_processor.h"
#include "definitions.h"

/* Defining the opcodes */
InstructionDefinition OPCODES[] = {
    {"mov", 0, 2, ALL_MODES, ALL_EXCEPT_IMMEDIATE},
    {"cmp", 1, 2, ALL_MODES, ALL_MODES},
    {"add", 2, 2, ALL_MODES, ALL_EXCEPT_IMMEDIATE},
    {"sub", 3, 2, ALL_MODES, ALL_EXCEPT_IMMEDIATE},
    {"lea", 4, 2, DIRECT_AND_REGISTER, ALL_EXCEPT_IMMEDIATE},
    {"clr", 5, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"not", 6, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"inc", 7, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"dec", 8, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"jmp", 9, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"bne", 10, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"jsr", 11, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"red", 12, 1, NO_MODES, ALL_EXCEPT_IMMEDIATE},
    {"prn", 13, 1, NO_MODES, ALL_MODES},
    {"rts", 14, 0, NO_MODES, NO_MODES},
    {"stop", 15, 0, NO_MODES, NO_MODES}};

/* Defining the registers */
char *REGISTERS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/* Defining the instructions */
char *INSTRUCTIONS[] = {".data", ".string", ".entry", ".extern", ".mat"};

InstructionDefinition *retrieve_instruction_set()
{
    return OPCODES;
}

/*
 * Internal helpers for tolerant operand parsing
 * - Allow spaces inside matrix brackets and around commas
 * - Still treat content outside operands as extraneous text
 */
static const char *skip_spaces_const(const char *p)
{
    while (p && *p && isspace((unsigned char)*p)) p++;
    return p;
}

static char *trim_copy_range(const char *start, const char *end)
{
    /* Trim leading/trailing spaces in [start,end) and return allocated copy */
    const char *s = start;
    const char *e = end;
    while (s < e && isspace((unsigned char)*s)) s++;
    while (e > s && isspace((unsigned char)*(e - 1))) e--;
    {
        size_t len = (size_t)(e - s);
        char *out = (char *)allocate_memory(len + 1);
        size_t i;
        if (out == NULL)
            return NULL;
        for (i = 0; i < len; i++) out[i] = s[i];
        out[len] = '\0';
        return out;
    }
}

/* Extract next operand token respecting bracket nesting; sets *out_next to position after comma or end */
static char *extract_operand_token(const char *p, const char **out_next)
{
    const char *q;
    int depth = 0;
    p = skip_spaces_const(p);
    if (*p == '\0') { if (out_next) *out_next = p; return NULL; }
    q = p;
    while (*q)
    {
        if (*q == '[') depth++;
        else if (*q == ']') { if (depth > 0) depth--; }
        else if (*q == COMMA_SIGN && depth == 0)
            break;
        q++;
    }
    /* q points at comma or string terminator */
    {
        char *tok = trim_copy_range(p, q);
        if (out_next)
        {
            if (*q == COMMA_SIGN) *out_next = q + 1; else *out_next = q;
        }
        return tok; /* may be empty string if only spaces; caller validates */
    }
}

int validate_macro_identifier(char *source_file, char *macro_identifier, int line_number)
{
    /* Checking if there is more than one name */
    if (contains_whitespace(macro_identifier))
    {
        log_syntax_error(Error_206, source_file, line_number);
        return 1; /* Indicates faliure */
    }
    /* Comparing the macro name with each of the system's reserved words */
    if (check_reserved_word_conflict(source_file, macro_identifier, line_number, REGULAR) != 0)
        return 1; /* Indicates failure */

    return 0; /* Indicates success */
}

int validate_label_identifier(char *label_identifier, Type label_type, Line *context, int *error_counter)
{
    char first_char;
    int i, label_name_len;
    Label *label;
    label_name_len = strlen(label_identifier);
    first_char = label_identifier[0];

    /* Checking if the current line is not an entry/extern instruction line */
    if (label_type == REGULAR)
    {
        label_identifier[label_name_len - 1] = STRING_TERMINATOR; /* Getting the label name without ':' */
        label_name_len -= 1;
    }
    /* Checking if the name is empty */
            if (*label_identifier == STRING_TERMINATOR)
    {
        log_syntax_error(label_type == REGULAR ? Error_219 : label_type == OPERAND ? Error_253
                                                                        : Error_234,
                           context->file_am_name, context->line_num);
        *error_counter = 1;
        return 1; /* Indicates label name is not valid */
    }
    /* Checking if the first character is an alphabetic */
    if (!isalpha(first_char))
    {
        if (identify_assembler_directive(label_identifier) != -1 && label_type == REGULAR)
        { /* Checking if the label name is an instruction in case of a non alphabetic first character */
            log_syntax_error(4, context->file_am_name, context->line_num);
            *error_counter = 1;
            return 1; /* Indicates label name is not valid */
        }
        log_syntax_error(label_type == REGULAR ? Error_220 : label_type == OPERAND ? Error_253
                                                                          : Error_236,
                             context->file_am_name, context->line_num);
        *error_counter = 1;
        return 1; /* Indicates label name is not valid */
    }
    /* Checking if the label name length is valid */
    if (label_name_len > MAX_LABEL_NAME_LENGTH)
    {
        log_syntax_error(Error_214, context->file_am_name, context->line_num);
        *error_counter = 1;
        return 1; /* Indicates label name is not valid */
    }
    /* Checking if the label name only contains alphabetic or numeric characters */
    for (i = 1; i < label_name_len - 1; i++)
    {
        if (!isalnum(label_identifier[i]))
        {
            log_syntax_error(Error_213, context->file_am_name, context->line_num);
            *error_counter = 1;
            return 1; /* Indicates label name is not valid */
        }
    }
    /* Checking if the label name is a macro name */
    if (is_macro_name(label_identifier) != NULL)
    {
        log_syntax_error(label_type == REGULAR ? Error_215 : label_type == OPERAND ? Error_262
                                                                          : Error_237,
                             context->file_am_name, context->line_num);
        *error_counter = 1;
        return 1; /* Indicates label name is not valid */
    }
    /* Checking if the label name is a reserved word */
    if (check_reserved_word_conflict(context->file_am_name, label_identifier, context->line_num, label_type) != 0)
    {
        *error_counter = 1;
        return 1; /* Indicates label name is not valid */
    }
    if (label_type == OPERAND)
        return 0; /* Finished validation for type "operand" */

    /* Checking if the label name had already been defined */
    label = is_label_name(label_identifier);
    if (label != NULL)
    {
        if (label_type == ENTRY)
        {
            if (label->type != ENTRY)
            {
                log_syntax_error(Error_238, context->file_am_name, context->line_num);
                *error_counter = 1;
                return 1; /* Indicates label name is not valid */
            }
            if (label->type == ENTRY)
            {
                printf(" WARNING | File \"%s\" at line %d | Instructions \".entry\" or"
                       " \".extern\" duplicate declarations will be ignored\n",
                       context->file_am_name, context->line_num);
                return 1; /* Scanning line finished */
            }
            label->type = ENTRY; /* Updating label type */
            return 1;            /* Scanning line finished */
        }
        if (label_type == EXTERN)
        {
            if (label->type != EXTERN)
            {
                log_syntax_error(Error_239, context->file_am_name, context->line_num);
                *error_counter = 1;
                return 1; /* Indicates label name is not valid */
            }
            printf(" WARNING | File \"%s\" at line %d | Instructions \".entry\" or"
                   " \".extern\" duplicate declarations will be ignored\n",
                   context->file_am_name, context->line_num);
            return 1; /* Scanning line finished */
        }
        /* If this line was reached then label type is "REGULAR" */
        if (label->type == EXTERN)
        {
            log_syntax_error(Error_223, context->file_am_name, context->line_num);
            *error_counter = 1;
            return 1; /* Indicates label name is not valid */
        }
        if (label->type == REGULAR)
        {
            log_syntax_error(Error_218, context->file_am_name, context->line_num);
            *error_counter = 1;
            return 1; /* Indicates label name is not valid */
        }
        if (label->type == ENTRY && label->location == TBD)
        {
            /* Entry label found but not yet defined - this is allowed */
            return -1; /* Signal to update the existing entry label */
        }
        if (label->type == OPERAND)
        {
            /* Don't remove operand labels - allow creating regular labels with same name */
            return 0; /* Allow creating the regular label */
        }

        remove_label(label);
        return -1; /* Special case - signaling to create a new label of type "entry" */
    }
    return 0; /* Indicates label name is valid */
}

int lookup_instruction_opcode(char *token)
{
    int i;

    if (token == NULL) /* Indicates string is not an opcode name */
        return -1;

    /* Comparing the string with each of the system opcodes */
    for (i = 0; i < TOTAL_OPCODES; i++)
    {
        if (strcmp(token, OPCODES[i].mnemonic) == 0)
        {
            return i; /* Returning the index of the matching opcode */
        }
    }
    return -1; /* Indicates string is not an opcode name */
}

int parse_register_operand(char *register_token)
{
    int i;

    if (register_token == NULL) /* Indicates string is not a register name */
        return -1;

    /* Comparing the string with each of the system registers */
    for (i = 0; i < TOTAL_REGISTERS; i++)
    {
        if (strcmp(register_token, REGISTERS[i]) == 0)
        {
            return i; /* Returning the index of the matching register */
        }
    }
    return -1; /* Indicates string is not a register name */
}

int identify_assembler_directive(char *directive_token)
{
    int i;

    if (directive_token == NULL) /* Indicates string is not an instruction name */
        return -1;

    /* Comparing the string with each of the system instructions */
    for (i = 0; i < TOTAL_INSTRUCTION_TYPES; i++)
    {
        if (strcmp(directive_token, INSTRUCTIONS[i]) == 0)
        {
            return i; /* Returning the index of the matching instruction */
        }
    }

    return -1; /* Indicates string is not an instruction name */
}

int determine_operand_addressing_mode(char *operand_text, Line *context, int *error_counter)
{
    char *endptr;
    long val;

    /* Immediate addressing (#number) */
    if (operand_text[0] == POUND_SIGN)
    {
        operand_text++;
        if (*operand_text == STRING_TERMINATOR)
        {
            log_syntax_error(Error_254, context->file_am_name, context->line_num);
            *error_counter = 1;
            return -1;
        }
        val = strtol(operand_text, &endptr, BASE_10);
        if (*endptr != STRING_TERMINATOR || endptr == operand_text)
        {
            log_syntax_error(Error_255, context->file_am_name, context->line_num);
            *error_counter = 1;
            return -1;
        }
        if (val < MIN_10_BIT_SIGNED_VALUE || val > MAX_10_BIT_SIGNED_VALUE)
        {
            log_syntax_error(Error_256, context->file_am_name, context->line_num);
            *error_counter = 1;
            return -1;
        }
        return IMMEDIATE;
    }

    /* Matrix addressing (LABEL[rX][rY]) - allow spaces inside brackets but not between "] [" */
    if (strchr(operand_text, LEFT_BRACKET) && strchr(operand_text, RIGHT_BRACKET))
    {
        /* Detect illegal spaces between consecutive brackets: "]  [" */
        {
            const char *p = operand_text;
            while ((p = strchr(p, RIGHT_BRACKET)) != NULL)
            {
                const char *q = p + 1;
                while (*q && isspace((unsigned char)*q)) q++;
                if (*q == '[')
                {
                    if (q != p + 1) /* there were spaces between ] and [ */
                    {
                        log_syntax_error(Error_251, context->file_am_name, context->line_num);
                        *error_counter = 1;
                        return -1;
                    }
                }
                p = q;
            }
        }

        /* Build a cleaned copy without spaces to simplify parsing */
        {
            size_t len = strlen(operand_text);
            char *clean = (char *)allocate_memory(len + 1);
            size_t i = 0, j = 0;
            if (clean == NULL)
            {
                fclose(context->file);
                free_line(context);
                exit(1);
            }
            for (; i < len; i++)
            {
                if (!isspace((unsigned char)operand_text[i]))
                    clean[j++] = operand_text[i];
            }
            clean[j] = '\0';

            /* Now parse strictly without spaces */
            {
                char label_name[MAX_LABEL_NAME_LENGTH], row_reg[REGISTER_STRING_BUFFER_SIZE], col_reg[REGISTER_STRING_BUFFER_SIZE];
                int parsed = sscanf(clean, "%30[^[][%4[^]]][%4[^]]]", label_name, row_reg, col_reg);
                if (parsed == 3)
                {
                    int row_num = -1, col_num = -1;
                    if ((row_reg[0] == 'r' || row_reg[0] == 'R') && isdigit((unsigned char)row_reg[1]))
                        row_num = row_reg[1] - '0';
                    if ((col_reg[0] == 'r' || col_reg[0] == 'R') && isdigit((unsigned char)col_reg[1]))
                        col_num = col_reg[1] - '0';
                    if (row_num < MIN_REGISTER_NUMBER || row_num > MAX_REGISTER_NUMBER ||
                        col_num < MIN_REGISTER_NUMBER || col_num > MAX_REGISTER_NUMBER)
                    {
                        log_syntax_error(Error_251, context->file_am_name, context->line_num);
                        *error_counter = 1;
                        deallocate_memory(clean);
                        return -1;
                    }
                    deallocate_memory(clean);
                    return MATRIX;
                }
                deallocate_memory(clean);
            }
        }
        log_syntax_error(Error_251, context->file_am_name, context->line_num);
        *error_counter = 1;
        return -1;
    }

    /* Register addressing (rX or *rX) */
    if (operand_text[0] == ASTERISK_SIGN)
    {
        operand_text++;
        if (*operand_text == STRING_TERMINATOR)
        {
            log_syntax_error(Error_257, context->file_am_name, context->line_num);
            *error_counter = 1;
            return -1;
        }
        if (parse_register_operand(operand_text) == -1)
        {
            log_syntax_error(Error_258, context->file_am_name, context->line_num);
            *error_counter = 1;
            return -1;
        }
        return DIRECT_REGISTER;
    }

    /* Direct register addressing (rX) */
    if (parse_register_operand(operand_text) != -1)
        return DIRECT_REGISTER;

    /* Potential label - validate */
    if (validate_label_identifier(operand_text, OPERAND, context, error_counter) != 0)
        return -1;

    return DIRECT;
}

int check_reserved_word_conflict(char *source_file, char *identifier, int line_number, Type identifier_type)
{
    int len = strlen(source_file);
    
    /* Comparing the string with each of the system's reserved words and determining the error message */
    if (lookup_instruction_opcode(identifier) != -1)
    { /* Function returns the index of the matching word or -1 if no word matched */
        if (strcmp(&source_file[len - FILE_EXTENSION_LENGTH], ".as") == 0 && identifier_type != OPERAND)
        { /* Indicates this is a macro name validation */
            log_syntax_error(Error_204, source_file, line_number);
            return 1; /* Indicates the name is invalid */
        }
        log_syntax_error(identifier_type == OPERAND ? Error_262 : identifier_type == REGULAR ? Error_216
                                                                        : Error_236,
                           source_file, line_number);
        return 1; /* Indicates the name is invalid */
    }
    if (parse_register_operand(identifier) != -1)
    {
        if (strcmp(&source_file[len - FILE_EXTENSION_LENGTH], ".as") == 0 && identifier_type != OPERAND)
        { /* Indicates this is a macro name validation */
            log_syntax_error(Error_205, source_file, line_number);
            return 1; /* Indicates the name is invalid */
        }
        log_syntax_error(identifier_type == OPERAND ? Error_262 : identifier_type == REGULAR ? Error_217
                                                                        : Error_236,
                           source_file, line_number);
        return 1; /* Indicates the name is invalid */
    }
    if (identify_assembler_directive(identifier) != -1)
    {
        if (strcmp(&source_file[len - FILE_EXTENSION_LENGTH], ".as") == 0 && identifier_type != OPERAND)
        { /* Indicates this is a macro name validation */
            log_syntax_error(Error_204, source_file, line_number);
            return 1; /* Indicates the name is invalid */
        }
        log_syntax_error(identifier_type == OPERAND ? Error_262 : identifier_type == REGULAR ? Error_216
                                                                        : Error_236,
                           source_file, line_number);
        return 1; /* Indicates the name is invalid */
    }
    if (strcmp(identifier, "mcro") == 0)
    {
        if (strcmp(&source_file[len - FILE_EXTENSION_LENGTH], ".as") == 0 && identifier_type != OPERAND)
        { /* Indicates this is a macro name validation */
            log_syntax_error(Error_210, source_file, line_number);
            return 1; /* Indicates the name is invalid */
        }
        log_syntax_error(identifier_type == OPERAND ? Error_262 : identifier_type == REGULAR ? Error_221
                                                                        : Error_236,
                           source_file, line_number);
        return 1; /* Indicates the name is invalid */
    }
    if (strcmp(identifier, "endmcro") == 0)
    {
        if (strcmp(&source_file[len - FILE_EXTENSION_LENGTH], ".as") == 0 && identifier_type != OPERAND)
        { /* Indicates this is a macro name validation */
            log_syntax_error(Error_211, source_file, line_number);
            return 1; /* Indicates the name is invalid */
        }
        log_syntax_error(identifier_type == OPERAND ? Error_262 : identifier_type == REGULAR ? Error_222
                                                                        : Error_236,
                           source_file, line_number);
        return 1; /* Indicates the name is invalid */
    }
    return 0; /* Indicates the name is valid */
}

int parse_assembler_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *parse_position, char *current_token, int *error_counter)
{
    int curr_word_len = strlen(current_token);

    /* Checking for a potential instruction */
    switch (identify_assembler_directive(current_token))
    {
    case 0:
        parse_position += curr_word_len; /* Skipping the first word ".data" */
        process_data_directive(data_segment, memory_usage, data_counter, context, parse_position, error_counter);
        return 1; /* Scanning line finished */
    case 1:
        parse_position += curr_word_len; /* Skipping the first word ".string" */
        process_string_directive(data_segment, memory_usage, data_counter, context, parse_position, error_counter);
        return 1; /* Scanning line finished */
    case 2:
        parse_position += curr_word_len; /* Skipping the first word ".entry" */
        process_entry_directive(context, parse_position, error_counter);
        return 1; /* Scanning line finished */
    case 3:
        parse_position += curr_word_len; /* Skipping the first word ".extern" */
        process_extern_directive(context, parse_position, error_counter);
        return 1; /* Scanning line finished */
    case 4:       /* Skipping the first word ".mat" */
        parse_position += curr_word_len;
        process_matrix_directive(data_segment, memory_usage, data_counter, context, parse_position, error_counter);
        return 1;
    default:
        return 0; /* Indicates line is not an "instruction" line, continue scanning */
    }
}

int parse_executable_instruction(unsigned short *instruction_segment, int *memory_usage, int *instruction_counter, Line *context, char *parse_position, char *current_token, int *error_counter)
{
    int curr_word_len = strlen(current_token), ind;

    /* Checking for a potential operation */
    ind = lookup_instruction_opcode(current_token);
    if (ind != -1)
    { /* Indicates line is an "operation" line */
        /* Updating label properties */
        if (context->label != NULL)
        {
            context->label->address = *instruction_counter + MEMORY_START_ADDRESS;
            /* Check if address exceeds memory capacity */
            if (context->label->address > MAX_ARRAY_CAPACITY + MEMORY_START_ADDRESS)
            {
                log_system_error(Error_105);
                *error_counter = 1;
                return 1;
            }
            context->label->location = CODE;
        }
        parse_position += curr_word_len; /* Skipping the first word */

        generate_instruction_machine_code(instruction_segment, memory_usage, instruction_counter, context, parse_position, ind, error_counter); /* Validating operation */
        return 1;                                                       /* Scanning line finished */
    }
    return 0; /* Indicates line is not an "operation" line, continue scanning */
}

int validate_addressing_mode_compatibility(Line *context, int addressing_mode, int instruction_index, int operand_position, int *error_counter)
{
    /* Getting the legal methods for the current operation */
    if (operand_position == 1)
    { /* Indicates operand is of type "destination" */
        switch (OPCODES[instruction_index].destination_modes)
        {
        case ALL_EXCEPT_IMMEDIATE:
            if (addressing_mode == IMMEDIATE)
            {
                log_syntax_error(Error_248, context->file_am_name, context->line_num);
                *error_counter = 1;
                return 1; /* Indicates method is illegal */
            }
            break;
        case DIRECT_AND_REGISTER:
            if (addressing_mode == IMMEDIATE || addressing_mode == MATRIX)
            {
                log_syntax_error(Error_248, context->file_am_name, context->line_num);
                *error_counter = 1;
                return 1; /* Indicates method is illegal */
            }
            break;
        default:
            return 0; /* Indicates method is legal */
        }
    }
    /* If this line was reached then operand is of type "source" */
    if (OPCODES[instruction_index].source_modes == DIRECT_ONLY)
    {
        if (addressing_mode != DIRECT)
        {
            log_syntax_error(Error_249, context->file_am_name, context->line_num);
            *error_counter = 1;
            return 1; /* Indicates method is illegal */
        }
    }
    return 0; /* Indicates method is legal */
}

void process_data_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *value_list, int *error_counter)
{
    /* Checking if there are no parameters */
    if (*value_list == STRING_TERMINATOR)
    {
        if (context->label != NULL)
            remove_last_label();
        log_syntax_error(Error_231, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    /* Analyzing input numbers */
    parse_and_encode_numeric_data(data_segment, memory_usage, data_counter, context, value_list, error_counter);
}

void process_string_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *string_literal, int *error_counter)
{
    char *trimmed_line;
    int trimmed_line_len, i;

    /* Checking for a parameter */
    if (*string_literal == STRING_TERMINATOR)
    {
        if (context->label != NULL)
            remove_last_label();
        log_syntax_error(Error_233, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    trimmed_line = trim_whitespace(string_literal);
    trimmed_line_len = strlen(trimmed_line);

    /* Checking for double quotes at the start and the end of the string */
    if (trimmed_line[0] != QUOTATION_MARK || trimmed_line[trimmed_line_len - 1] != QUOTATION_MARK)
    {
        if (context->label != NULL)
            remove_last_label();
        log_syntax_error(Error_232, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    /* Checking if the string is empty */
    if (strlen(trimmed_line) == BINARY_BASE)
    { /* Indicates string contains only double quotes */
        printf(" File \"%s\" at line %d | Instruction \".string\" parameter"
               " is an empty string\n",
               context->file_am_name, context->line_num);
    }
    trimmed_line[trimmed_line_len - 1] = STRING_TERMINATOR;
    trimmed_line++;
    trimmed_line_len = strlen(trimmed_line);

    /* Updating label properties */
    if (context->label != NULL)
    {
        context->label->address = *data_counter;
        context->label->location = DATA;
    }
    /* Adding machine code to data array */
    for (i = 0; i < trimmed_line_len; i++)
    {
        if (*memory_usage + 1 == MAX_ARRAY_CAPACITY)
        { /* Checking if memory limit was reached (+1 to account for the null-terminator) */
            log_system_error(Error_105);
            *error_counter = 1;
            (*memory_usage)++; /* Incrementing usage count so the next iteration will not print another error message */
            return;     /* Scanning line finished */
        }
        if (*memory_usage + 1 > MAX_ARRAY_CAPACITY)
        {           /* Checking if memory limit was exceeded */
            return; /* Scanning line finished */
        }
        /* Getting the ASCII value by converting 'char' type to 'int' and then adding code */
        add_data(data_segment, data_counter, (int)trimmed_line[i]);
        *memory_usage += 1; /* Incrementing usage count */
    }
    add_data(data_segment, data_counter, 0); /* Adding the null-terminator */
    *memory_usage += 1;                /* Incrementing usage count */
}

void process_entry_directive(Line *context, char *label_list, int *error_counter)
{
    Label *label;
    char *trimmed_line;

    /* Checking if a label was already declared at the current line */
    if (context->label != NULL)
    {
        printf("File \"%s\" at line %d | Label defined at the start of an"
               " \".entry\" or \".extern\" instruction line will be ignored\n",
               context->file_am_name, context->line_num);
        remove_label(context->label);
    }
    /* Checking if there is no label declaration */
    if (*label_list == STRING_TERMINATOR)
    {
        log_syntax_error(Error_234, context->file_am_name, context->line_num);
        *error_counter = 1;
        return; /* Scanning line finished */
    }
    /* Checking if there is more than one label declaration */
    trimmed_line = trim_whitespace(label_list);
    if (contains_whitespace(trimmed_line))
    {
        log_syntax_error(Error_235, context->file_am_name, context->line_num);
        *error_counter = 1;
        return; /* Scanning line finished */
    }
    if (validate_label_identifier(trimmed_line, ENTRY, context, error_counter) != 0)
        return; /* Scanning line finished */

    label = add_label(trimmed_line, 0, ENTRY, TBD);
    if (label == NULL)
    { /* Indicates memory allocation failed */
        fclose(context->file);
        free_line(context);
        free_labels();
        free_macros();
        free_all_memory();
        exit(1); /* Exiting program */
    }
    context->label = label; /* Setting the label pointer of struct line to the new entry label */
}

void process_extern_directive(Line *context, char *symbol_list, int *error_counter)
{
    Label *label;
    char *trimmed_line;

    /* Checking if a label was already declared at the current line */
    if (context->label != NULL)
    {
        printf("File \"%s\" at line %d | Label defined at the start of an"
               " \".entry\" or \".extern\" instruction line will be ignored\n",
               context->file_am_name, context->line_num);
        remove_label(context->label);
    }
    /* Checking if there is no label declaration */
    if (*symbol_list == STRING_TERMINATOR)
    {
        log_syntax_error(Error_234, context->file_am_name, context->line_num);
        *error_counter = 1;
        return; /* Scanning line finished */
    }
    /* Checking if there is more than one label declaration */
    trimmed_line = trim_whitespace(symbol_list);
    if (contains_whitespace(trimmed_line))
    {
        log_syntax_error(Error_235, context->file_am_name, context->line_num);
        *error_counter = 1;
        return; /* Scanning line finished */
    }
    if (validate_label_identifier(trimmed_line, EXTERN, context, error_counter) != 0)
        return; /* Scanning line finished */

    label = add_label(trimmed_line, 0, EXTERN, TBD);
    if (label == NULL)
    { /* Indicates memory allocation failed */
        fclose(context->file);
        free_line(context);
        free_labels();
        free_macros();
        free_all_memory();
        exit(1); /* Exiting program */
    }
    context->label = label; /* Setting the label pointer of struct line to the new entry label */
}

void process_matrix_directive(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *matrix_definition, int *error_counter)
{
    int rows = 0, cols = 0, count = 0, i, num;
    char *values_part;
    char *token;
    char values_copy[MAX_SOURCE_LINE_LENGTH];

    /* Checking if there are no parameters */
    if (*matrix_definition == STRING_TERMINATOR)
    {
        if (context->label != NULL)
            remove_last_label();
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }

    /* Skip whitespace */
    while (*matrix_definition && isspace((unsigned char)*matrix_definition))
        matrix_definition++;

    /* Expect '[' then rows */
    if (*matrix_definition != '[')
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    matrix_definition++;
    if (!isdigit((unsigned char)*matrix_definition))
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    rows = atoi(matrix_definition);

    /* Find closing bracket */
    while (*matrix_definition && *matrix_definition != ']') matrix_definition++;
    if (*matrix_definition != ']')
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    matrix_definition++;

    /* Expect '[' then cols */
    while (*matrix_definition && isspace((unsigned char)*matrix_definition)) matrix_definition++;
    if (*matrix_definition != '[')
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    matrix_definition++;
    if (!isdigit((unsigned char)*matrix_definition))
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    cols = atoi(matrix_definition);

    while (*matrix_definition && *matrix_definition != ']') matrix_definition++;
    if (*matrix_definition != ']')
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
    matrix_definition++;

    /* Validate */
    if (rows <= 0 || cols <= 0)
    {
        log_syntax_error(Error_250, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }

    /* Skip spaces to values */
    while (*matrix_definition && isspace((unsigned char)*matrix_definition)) matrix_definition++;
    values_part = matrix_definition;

    /* Count values */
    strncpy(values_copy, values_part, MAX_SOURCE_LINE_LENGTH - 1);
    values_copy[MAX_SOURCE_LINE_LENGTH - 1] = '\0';
    token = strtok(values_copy, ",");
    while (token != NULL)
    {
        count++;
        token = strtok(NULL, ",");
    }

    /* Too many values */
    if (count > rows * cols)
    {
        log_syntax_error(Error_252, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }

    /* Updating label properties */
    if (context->label != NULL)
    {
        context->label->address = *data_counter;
        context->label->location = DATA;
    }

    /* Parse and store matrix values */
    token = strtok(values_part, ",");
    for (i = 0; i < rows * cols; i++)
    {
        if (*memory_usage == MAX_ARRAY_CAPACITY)
        { /* Checking if memory limit was reached */
            log_system_error(Error_105);
            *error_counter = 1;
            (*memory_usage)++; /* Incrementing usage count so the next iteration will not print another error message */
            return;     /* Scanning line finished */
        }
        if (*memory_usage > MAX_ARRAY_CAPACITY)
        {           /* Checking if memory limit was exceeded */
            return; /* Scanning line finished */
        }

        if (token != NULL)
        {
            num = atoi(token);
            if (num < MIN_10_BIT_SIGNED_VALUE || num > MAX_10_BIT_SIGNED_VALUE)
            {
                log_syntax_error(Error_230, context->file_am_name, context->line_num);
                *error_counter = 1;
                num = 0;
            }
            add_data(data_segment, data_counter, num);
            *memory_usage += 1; /* Incrementing usage count */
            token = strtok(NULL, ",");
        }
        else
        {
            add_data(data_segment, data_counter, 0); /* fill missing */
            *memory_usage += 1; /* Incrementing usage count */
        }
    }
}

void generate_instruction_machine_code(unsigned short *instruction_segment, int *memory_usage, int *instruction_counter, Line *context, char *operand_list, int instruction_index, int *error_counter)
{
    char *opernad, *second_operand;
    int operands_num = OPCODES[instruction_index].operand_count, method, method_2;
    unsigned short word = 0;
            word |= (instruction_index << OPCODE_SHIFT_POSITION) | ARE_ABSOLUTE;

    /* Analyzing opernads */
    switch (operands_num)
    {
    case 0:
        if (operand_list[0] != STRING_TERMINATOR)
        { /* Checking if there is a extraneous text */
            log_syntax_error(Error_240, context->file_am_name, context->line_num);
            *error_counter = 1;
            return; /* Scanning line finished */
        }
        add_instruction(instruction_segment, memory_usage, instruction_counter, word, error_counter); /* Adding machine code */
        return;                                                    /* Scanning line finished */
    case 1:
        {
            const char *next;
            operand_list = (char *)skip_spaces_const(operand_list);
            if (*operand_list == STRING_TERMINATOR)
            { /* missing operand */
                log_syntax_error(Error_241, context->file_am_name, context->line_num);
                *error_counter = 1;
                return;
            }
            if (*operand_list == COMMA_SIGN)
            { /* illegal leading comma */
                log_syntax_error(Error_244, context->file_am_name, context->line_num);
                *error_counter = 1;
                return;
            }
            opernad = extract_operand_token(operand_list, &next);
            if (opernad == NULL || opernad[0] == STRING_TERMINATOR)
            { /* empty token */
                log_syntax_error(Error_241, context->file_am_name, context->line_num);
                *error_counter = 1;
                if (opernad) deallocate_memory(opernad);
                return;
            }
            next = skip_spaces_const(next);
            if (*next != STRING_TERMINATOR)
            { /* extraneous text beyond operand */
                log_syntax_error(Error_243, context->file_am_name, context->line_num);
                *error_counter = 1;
                deallocate_memory(opernad);
                return;
            }
            method = determine_operand_addressing_mode(opernad, context, error_counter);
        }
        if (method == -1)
        {
            deallocate_memory(opernad);
            return; /* Scanning line finished */
        }
        if (validate_addressing_mode_compatibility(context, method, instruction_index, operands_num, error_counter) != 0)
        {
            deallocate_memory(opernad);
            return; /* Scanning line finished */
        }
        process_one_operand(instruction_segment, memory_usage, instruction_counter, context, method, opernad, instruction_index, error_counter);
        deallocate_memory(opernad);
        return; /* Scanning line finished */
    case 2:
        {
            const char *next;
            const char *after_first;
            operand_list = (char *)skip_spaces_const(operand_list);
            if (*operand_list == STRING_TERMINATOR)
            { /* missing both operands */
                log_syntax_error(Error_242, context->file_am_name, context->line_num);
                *error_counter = 1;
                return;
            }
            if (*operand_list == COMMA_SIGN)
            { /* illegal leading comma */
                log_syntax_error(Error_244, context->file_am_name, context->line_num);
                *error_counter = 1;
                return;
            }
            opernad = extract_operand_token(operand_list, &next);
            if (opernad == NULL || opernad[0] == STRING_TERMINATOR)
            {
                log_syntax_error(Error_241, context->file_am_name, context->line_num);
                *error_counter = 1;
                if (opernad) deallocate_memory(opernad);
                return;
            }
            after_first = skip_spaces_const(next);
            if (*after_first != COMMA_SIGN && *(next - 1) != COMMA_SIGN)
            {
                /* extract_operand_token sets next to after comma or end; if no comma existed, it's syntax */
                log_syntax_error(Error_247, context->file_am_name, context->line_num);
                *error_counter = 1;
                deallocate_memory(opernad);
                return;
            }
            /* Ensure we move to content after comma */
            if (*next == COMMA_SIGN)
                next++;
            next = skip_spaces_const(next);
            if (*next == COMMA_SIGN)
            { /* consecutive commas */
                log_syntax_error(Error_246, context->file_am_name, context->line_num);
                *error_counter = 1;
                deallocate_memory(opernad);
                return;
            }
            second_operand = extract_operand_token(next, &after_first);
            if (second_operand == NULL || second_operand[0] == STRING_TERMINATOR)
            {
                log_syntax_error(Error_241, context->file_am_name, context->line_num);
                *error_counter = 1;
                deallocate_memory(opernad);
                if (second_operand) deallocate_memory(second_operand);
                return;
            }
            after_first = skip_spaces_const(after_first);
            if (*after_first != STRING_TERMINATOR)
            { /* extraneous text after two operands */
                log_syntax_error(Error_245, context->file_am_name, context->line_num);
                *error_counter = 1;
                deallocate_memory(opernad);
                deallocate_memory(second_operand);
                return;
            }
            method = determine_operand_addressing_mode(opernad, context, error_counter);
            method_2 = determine_operand_addressing_mode(second_operand, context, error_counter);
        }
        if (method == -1 || method_2 == -1)
        {
            deallocate_memory(opernad);
            deallocate_memory(second_operand);
            return; /* Scanning line finished */
        }
        if (validate_addressing_mode_compatibility(context, method, instruction_index, operands_num, error_counter) != 0 || /* Checking if the addressing method is legal */
            validate_addressing_mode_compatibility(context, method_2, instruction_index, operands_num - 1, error_counter) != 0)
        { /* operands_num-1 to signal that opernd is of type "destination" */
            deallocate_memory(opernad);
            deallocate_memory(second_operand);
            return; /* Scanning line finished */
        }
        process_two_operands(instruction_segment, memory_usage, instruction_counter, context, opernad, second_operand, instruction_index, error_counter);
        deallocate_memory(opernad);
        deallocate_memory(second_operand);
    }
}

void parse_and_encode_numeric_data(unsigned short *data_segment, int *memory_usage, int *data_counter, Line *context, char *numeric_list, int *error_counter)
{
    int *num_array;
    int num_count = 0, i = 0;

    num_array = get_numbers(context, numeric_list, &num_count, error_counter);
    if (num_array == NULL)
    {
        if (context->label != NULL)
            remove_last_label();
        return;
    }
    /* Updating label properties */
    if (context->label != NULL)
    {
        context->label->address = *data_counter;
        context->label->location = DATA;
    }
    /* Adding machine code to data array */
    for (; i < num_count; i++)
    {
        if (*memory_usage == MAX_ARRAY_CAPACITY)
        { /* Checking if memory limit was reached */
            log_system_error(Error_105);
            *error_counter = 1;
            (*memory_usage)++; /* Incrementing usage count so the next iteration will not print another error message */
            return;     /* Scanning line finished */
        }
        if (*memory_usage > MAX_ARRAY_CAPACITY)
        {           /* Checking if memory limit was exceeded */
            return; /* Scanning line finished */
        }
        add_data(data_segment, data_counter, num_array[i]); /* Adding machine code */
        (*memory_usage)++;                            /* Incrementing usage count */
    }
    deallocate_memory(num_array);
}
