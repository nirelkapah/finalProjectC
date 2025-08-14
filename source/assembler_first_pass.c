/**
 * This is the first pass file, which is the first step in the assembler analysis process.
 * This file handles the scanning and storing of instructions, operations, and labels,
 * converting them into machine code while handling all types off potential errors.
 * If no errors are detected, it proceeds with the second pass.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler_first_pass.h"
#include "validator.h"
#include "error_handler.h"
#include "macro_handler.h"
#include "labels.h"
#include "utils.h"
#include "assembler_second_pass.h"
#include "definitions.h"

int first_pass(char *file_name)
{
    unsigned short code[MAX_ARRAY_CAPACITY] = {0}, data[MAX_ARRAY_CAPACITY] = {0}; /* Initializing machine code arrays */
    int IC = 0, DC = 0;

    /* Getting the new file name */
    char *file_am_name = change_extension(file_name, ".am");

    /* Scanning the file */
    if (scan_text(file_am_name, code, data, &IC, &DC) != 0)
    {
        free_labels();
        free_macros();
        free_all_memory();
        return 1; /* Indicates faliure */
    }
    free_macros(); /* Macros are no longer needed */

    printf("--- First pass passed successfully ---\n");

    /* Starting second pass */
    if (second_pass(file_am_name, code, data, &IC, &DC) != 0)
    {
        free_labels();
        free_all_memory();
        return 1; /* Indicates faliure */
    }
    deallocate_memory(file_am_name);
    return 0; /* Indicates success */
}

int scan_text(char *file_am_name, unsigned short *code, unsigned short *data, int *IC, int *DC)
{
    char temp[MAX_SOURCE_LINE_LENGTH + 1]; /* +1 to accommodate '\0' */
    int Usage = 0, errors_found = 0, line_count = 0;
    char *trimmed_line;
    Line *line;

    FILE *file_am = fopen(file_am_name, "r");
    if (file_am == NULL)
    { /* Failed to open file for reading */
        log_system_error(Error_103);
        free_macros();
        free_all_memory();
        exit(1); /* Exiting program */
    }
    /* Reading line by line */
    while (fgets(temp, MAX_SOURCE_LINE_LENGTH + 1, file_am))
    {
        line_count++;

        /* Checking if the current line a comment */
        if (temp[0] == SEMICOLON)
            continue; /* Skipping to the next line */

        /* Trimming leading and trailing whitespace characters */
        trimmed_line = trim_whitespace(temp);

        /* Checking if the current line is a empty */
        if (strlen(trimmed_line) == 0)
            continue; /* Skipping to the next line */

        line = create_line(file_am, file_am_name, trimmed_line, line_count);
        if (line == NULL)
        {
            fclose(file_am);
            free_labels();
            free_macros();
            free_all_memory();
            exit(1); /* Exiting program */
        }
        scan_word(code, data, &Usage, IC, DC, line, &errors_found);
        free_line(line);
    }
    fclose(file_am);
    return errors_found;
}

void scan_word(unsigned short *code, unsigned short *data, int *Usage, int *IC, int *DC, Line *line, int *errors_found)
{
    char *ptr = line->content;
    char *current_word, *temp;
    int curr_word_len, len, res;
    Label *label;

    /* Getting the first word */
    current_word = get_first_word(line->content);
    if (current_word == NULL)
    {
        fclose(line->file);
        free_line(line);
        exit(1);
    }
    curr_word_len = strlen(current_word);

    /* Checking for a potential label definition */
    if (current_word[curr_word_len - 1] == COLON_SIGN)
    {
        res = validate_label_identifier(current_word, REGULAR, line, errors_found);
        if (res == 0)  /* New label */
        {
            label = add_label(current_word, 0, REGULAR, TBD);
            line->label = label;
        }
        else if (res == -1)  /* Existing entry label - find and update it */
        {
            label = is_label_name(current_word);
            if (label != NULL && label->type == ENTRY)
            {
                line->label = label;
                /* The address and location will be set later in the code */
            }
            else
            {
                deallocate_memory(current_word);
                return;
            }
        }
        else
        {
            deallocate_memory(current_word);
            return;
        }
        if (label == NULL)
        {
            fclose(line->file);
            free_line(line);
            free_labels();
            free_macros();
            free_all_memory();
            exit(1);
        }
        line->label = label;
        deallocate_memory(current_word);
    }

    /* Scanning the next word */
    if (line->label != NULL)
    {
        if (contains_whitespace(ptr))
        {
            while (*ptr != STRING_TERMINATOR && !isspace(*ptr))
                ptr++;
            while (*ptr != STRING_TERMINATOR && isspace(*ptr))
                ptr++;
            current_word = get_first_word(ptr);
            if (current_word == NULL)
            {
                fclose(line->file);
                free_line(line);
                exit(1);
            }

            if (identify_assembler_directive(current_word) == 0 || /* .data */
                identify_assembler_directive(current_word) == 1 || /* .string */
                identify_assembler_directive(current_word) == 4)   /* .mat */
            {
                line->label->address = *DC;
                line->label->location = DATA;
            }
            else
            {
                line->label->address = *IC;
                line->label->location = CODE;
            }
        }
        else
        {
            ptr[strlen(ptr) - 1] = STRING_TERMINATOR;
            log_syntax_error(Error_214, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;
        }
    }

    /* Checking for a potential instruction */
    if (parse_assembler_directive(data, Usage, DC, line, ptr, current_word, errors_found) != 0)
    {
        deallocate_memory(current_word);
        return;
    }

    /* Checking for a potential operation */
    if (parse_executable_instruction(code, Usage, IC, line, ptr, current_word, errors_found) != 0)
    {
        deallocate_memory(current_word);
        return;
    }

    /* Handling special cases */
    if (is_macro_name(current_word) != NULL)
    {
        log_syntax_error(Error_224, line->file_am_name, line->line_num);
        *errors_found = 1;
        deallocate_memory(current_word);
        return;
    }
    while (ptr && !isspace(*ptr))
        ptr++;
    while (ptr && isspace(*ptr))
        ptr++;
            len = strlen(current_word) + BINARY_BASE;
    temp = (char *)allocate_memory(len);
    if (temp == NULL)
    {
        fclose(line->file);
        free_line(line);
        exit(1);
    }
    temp[0] = PERIOD;
    strcpy(temp + 1, current_word);
    if (identify_assembler_directive(temp) != -1)
    {
        log_syntax_error(Error_259, line->file_am_name, line->line_num);
        *errors_found = 1;
        deallocate_memory(temp);
        deallocate_memory(current_word);
        return;
    }
    deallocate_memory(temp);
    log_syntax_error(Error_260, line->file_am_name, line->line_num);
    *errors_found = 1;
    deallocate_memory(temp);
    deallocate_memory(current_word);
}
