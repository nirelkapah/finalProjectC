/**
 * This file handles the machine-coding for the assembler and contains functions for
 * handling operands and adding data and instruction codes to their arrays.
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "machine_code.h"
#include "errors.h"
#include "analyzer.h"
#include "labels.h"
#include "macros.h"
#include "utility.h"
#include "definitions.h"

void add_data_code(unsigned short *data, int *DC, int number)
{
    /* Store as 10-bit 2's complement */
    unsigned short word = (unsigned short)(number & MASK_10_BITS);

    data[*DC] = word;
    (*DC)++;
}

void add_instruction_code(unsigned short *code, int *Usage, int *IC, unsigned short word, int *errors_found)
{
    if (*Usage == MAX_ARRAY_CAPACITY)
    {
        print_system_error(Error_73);
        *errors_found = 1;
        (*Usage)++;
        return;
    }
    if (*Usage > MAX_ARRAY_CAPACITY)
        return;

    code[*IC] = (word & MASK_10_BITS);
    (*IC)++;
    *Usage += 1;
}

void process_operation_code(unsigned short *code, int *Usage, int *IC, Line *line, int method, char *operand, int operands_num, int *errors_found)
{
    unsigned short word = 0, temp = 0;
    Label *added_label;

    switch (method)
    {
    case IMMEDIATE:
    {
        int immediate_val;
        operand++;
        /* Check immediate value range (can be negative) */
        immediate_val = atoi(operand);
        if (immediate_val < MIN_10_BIT_SIGNED_VALUE || immediate_val > MAX_10_BIT_SIGNED_VALUE)
        {
            print_syntax_error(Error_62, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;
        }
        word |= ARE_ABSOLUTE;
        temp = (unsigned short)(immediate_val & MASK_8_BITS); /* 8-bit immediate value */
        word |= (temp << IMMEDIATE_VALUE_SHIFT_POSITION); /* Bits 9-2 contain the immediate value */
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;
    }

    case DIRECT:
        /* Store the operand label for second pass resolution */
        added_label = add_label(operand, *IC, OPERAND, TBD);
        if (added_label == NULL)
        {
            fclose(line->file);
            free_line(line);
            free_labels();
            free_macros();
            free_all_memory();
            exit(1);
        }
        /* Placeholder for second pass resolution - address will be filled in second pass */
        word |= ARE_PLACEHOLDER_SIGNAL; /* Signal for second pass to resolve */
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case DIRECT_REGISTER:
        /* Handle both direct and indirect register addressing */
        if (operand[0] == ASTERISK_SIGN) {
            operand++; /* Skip the '*' for indirect addressing */
        }
        word |= ARE_ABSOLUTE;
        word |= (which_regis(operand) << (operands_num == 1 ? DESTINATION_REGISTER_SHIFT_POSITION : SOURCE_REGISTER_SHIFT_POSITION));
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case MATRIX:
    {
        /* Store the full matrix expression for second pass parsing */
        if (add_label(operand, *IC, OPERAND, TBD) == NULL)
        {
            print_system_error(Error_1);
            *errors_found = 1;
            return;
        }
        /* Matrix addressing: add placeholders for second pass resolution */
        add_instruction_code(code, Usage, IC, ARE_PLACEHOLDER_SIGNAL, errors_found); /* Base address placeholder */
        add_instruction_code(code, Usage, IC, 0, errors_found); /* Register combination placeholder */
        return;
    }

    default:
        /* Unknown addressing method */
        print_syntax_error(Error_69, line->file_am_name, line->line_num);
        *errors_found = 1;
        return;
    }
}

void handle_one_operand(unsigned short *code, int *Usage, int *IC, Line *line, int method, char *operand, int ind, int *errors_found)
{
    unsigned short word = 0;
    Op_Code *opcodes = get_opcodes();
    int dst_encoding;

    word |= (ind << OPCODE_SHIFT_POSITION) | ARE_ABSOLUTE;
    /* Addressing method is already in correct 2-bit format */
    dst_encoding = method;
    word |= (dst_encoding << DESTINATION_OPERAND_SHIFT_POSITION);
    add_instruction_code(code, Usage, IC, word, errors_found);

    process_operation_code(code, Usage, IC, line, method, operand, opcodes[ind].operands_num, errors_found);
}

void handle_two_operands(unsigned short *code, int *Usage, int *IC, Line *line, char *operand, char *second_operand, int ind, int *errors_found)
{
    unsigned short word = 0, second_word = 0;
    Op_Code *opcodes = get_opcodes();
    int method, method_2;
    int dst_encoding, src_encoding;

    method = which_addressing_method(operand, line, errors_found);
    method_2 = which_addressing_method(second_operand, line, errors_found);

    word |= (ind << OPCODE_SHIFT_POSITION) | ARE_ABSOLUTE;
    /* Addressing methods are already in correct 2-bit format */
    dst_encoding = method_2;
    src_encoding = method;
    
    word |= (dst_encoding << DESTINATION_OPERAND_SHIFT_POSITION);
    word |= (src_encoding << SOURCE_OPERAND_SHIFT_POSITION);
    add_instruction_code(code, Usage, IC, word, errors_found);

    /* Combine if both operands are registers */
    if (method == DIRECT_REGISTER && method_2 == DIRECT_REGISTER)
    {
        /* Handle indirect addressing by skipping '*' if present */
        if (operand[0] == ASTERISK_SIGN)
            operand++;
        if (second_operand[0] == ASTERISK_SIGN)
            second_operand++;

        second_word |= ARE_ABSOLUTE;
        second_word |= (which_regis(operand) << SOURCE_REGISTER_SHIFT_POSITION);        /* First operand (r1) is source */
        second_word |= (which_regis(second_operand) << DESTINATION_REGISTER_SHIFT_POSITION); /* Second operand (r4) is destination */
        add_instruction_code(code, Usage, IC, second_word, errors_found);
        return;
    }

    /* Handle separately */
    process_operation_code(code, Usage, IC, line, method, operand, opcodes[ind].operands_num, errors_found);
    process_operation_code(code, Usage, IC, line, method_2, second_operand, opcodes[ind].operands_num - 1, errors_found);
}
