/**
 * This is the second pass file, which is the last step in the assembler process.
 * This file processes and updates uncoded label addresses, creates the relevant
 * output files (.ob, .ent, .ext), and manages potential errors.
 */
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "assembler_second_pass.h"
#include "error_handler.h"
#include "validator.h"
#include "definitions.h"
#include "labels_handler.h"
#include "utils.h"

int second_pass(char *file_am_name, unsigned short *code, unsigned short *data, int *IC, int *DC)
{
    char *file_ob_name, *file_ent_name, *file_ext_name;
    int errors_found = 0;

    /* Checking if all "entry" labels were defined */
    if (check_entry_labels(file_am_name) != 0)
    {
        errors_found = 1; /* Indicates failure */
    }
    /* Handling uncoded label addresses */
    update_data_labels(IC);
    if (code_operand_labels(file_am_name, code, IC) != 0)
    {
        free_labels();
        free_all_memory();
        return 1; /* Indicates failure */
    }
    /* Getting the object file name */
    file_ob_name = change_extension(file_am_name, ".ob");

    /* Creating the object file */
    create_ob_file(file_ob_name, code, data, IC, DC);

    /* Creating "file.ent" if there are "entry" labels */
    if (entry_exist() != 0)
    {
        file_ent_name = change_extension(file_am_name, ".ent");
        create_ent_file(file_ent_name);
        deallocate_memory(file_ent_name);
    }
    /* Creating "file.ext" if there are "extern" labels */
    if (extern_exist() != 0)
    {
        file_ext_name = change_extension(file_am_name, ".ext");
        create_ext_file(file_ext_name);
        deallocate_memory(file_ext_name);
    }
    deallocate_memory(file_ob_name);
    free_labels();
    printf("--- Second pass passed successfully ---\n");
    return errors_found;
}
int code_operand_labels(char *file_am_name, unsigned short *code, int *IC)
{
    int errors_found = 0;
    int i = 0;
    int j = 0;
    Label *operand_label;
    Label *label;
    unsigned short word;
    char *operand_name;
    


    while (i < *IC)
    {
        /* Skip non-placeholder words */
        while (j < *IC && (code[j] & ARE_PLACEHOLDER_SIGNAL) != ARE_PLACEHOLDER_SIGNAL)
        {
            j++;
        }

        if (j >= *IC)
        {
            return errors_found;
        }

        /* Get the next operand label */
        operand_label = get_opernad_label();
        if (operand_label == NULL)
        {
            return errors_found; /* No more operand labels */
        }



        operand_name = operand_label->name;
        word = 0;

        /* Matrix operand */
        if (strchr(operand_name, '[') != NULL)
        {
            char label_name[31];
            char row_reg_str[5];
            char col_reg_str[5];
            int row_reg = 0;
            int col_reg = 0;
            int parsed;

            parsed = sscanf(operand_name, "%30[^[][%4[^]]][%4[^]]]", label_name, row_reg_str, col_reg_str);
            if (parsed == 3)
            {

                label = is_label_defined(label_name);
                if (label != NULL)
                {
                    /* First word: base address */
                    /* Use the updated address for DATA labels */
                    int final_address = label->address;
                    if (label->location == DATA)
                    {
                        final_address = label->address; /* Already updated by update_data_labels */
                    }
                    else
                    {
                        final_address = label->address + MEMORY_START_ADDRESS;
                    }
                    

                    
                    word = (unsigned short)(final_address & MASK_8_BITS); /* 8-bit address */
                    word <<= IMMEDIATE_VALUE_SHIFT_POSITION; /* bits 9-2 = address */
                    word |= ARE_RELOCATABLE; /* ARE = 10 */
                    code[j] = word;

                    /* Validate and encode row register */
                    if ((row_reg_str[0] == 'r' || row_reg_str[0] == 'R') &&
                        isdigit((unsigned char)row_reg_str[1]))
                    {
                        row_reg = atoi(row_reg_str + 1);
                    }

                    /* Validate and encode column register */
                    if ((col_reg_str[0] == 'r' || col_reg_str[0] == 'R') &&
                        isdigit((unsigned char)col_reg_str[1]))
                    {
                        col_reg = atoi(col_reg_str + 1);
                    }

                    /* Second word: row/col registers */
                    if (j + 1 < *IC)
                    {
                        word = ((row_reg & MASK_4_BITS) << MATRIX_ROW_REGISTER_SHIFT) |
                               ((col_reg & MASK_4_BITS) << MATRIX_COLUMN_REGISTER_SHIFT) |
                               ARE_ABSOLUTE;
                        code[j + 1] = word;
                    }
                }
                else
                {
                    log_syntax_error(Error_261, file_am_name,
                                         operand_label->address + MEMORY_START_ADDRESS);
                    errors_found = 1;
                }
            }
            else
            {
                log_syntax_error(Error_251, file_am_name,
                                   operand_label->address + MEMORY_START_ADDRESS);
                errors_found = 1;
            }
        }

        /* Direct operand */
        else
        {   
            /* First try to find a defined label with this name */
            label = is_label_defined(operand_name);
            if (label == NULL)
            {
                /* If not found as defined, try to find any label with this name */
                label = is_label_name(operand_name);
            }

            
            if (label != NULL)
            {

                if (label->type == EXTERN)
                {
                    /* For external labels, put zeros in bits 2-9 since we don't know the address yet */
                    word = 0; /* Bits 9-2 = 0 */
                    word |= ARE_EXTERNAL; /* ARE = 01 */
                    if (add_label(label->name,
                                  operand_label->address + MEMORY_START_ADDRESS,
                                  EXTERN, CODE) == NULL)
                    {
                        free_labels();
                        free_all_memory();
                        exit(1);
                    }
                }
                else
                {
                    /* Use the updated address for DATA labels */
                    int final_address = label->address;
                    if (label->location == DATA)
                    {
                        final_address = label->address; /* Already updated by update_data_labels */
                    }
                    else
                    {
                        final_address = label->address + MEMORY_START_ADDRESS;
                    }
                    
                    word = (unsigned short)(final_address & MASK_8_BITS); /* 8-bit address */
                    word <<= IMMEDIATE_VALUE_SHIFT_POSITION; /* Bits 9-2 contain the memory address */
                    word |= ARE_RELOCATABLE; /* ARE = 10 */
                }
                code[j] = word;
            }
            else
            {
                log_syntax_error(Error_261, file_am_name,
                                     operand_label->address + MEMORY_START_ADDRESS);
                errors_found = 1;
            }
        }

        /* Move to next label */
        remove_label(operand_label);
        i++;
        j++;
    }

    return errors_found;
}
