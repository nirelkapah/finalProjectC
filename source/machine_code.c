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
    unsigned short word = (unsigned short)(number & MASK_10BIT);

    data[*DC] = word;
    (*DC)++;
}

void add_instruction_code(unsigned short *code, int *Usage, int *IC, unsigned short word, int *errors_found)
{
    if (*Usage == CAPACITY)
    {
        print_system_error(Error_73);
        *errors_found = 1;
        (*Usage)++;
        return;
    }
    if (*Usage > CAPACITY)
        return;

    code[*IC] = (word & MASK_10BIT);
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
        if (immediate_val < MIN_10BIT || immediate_val > MAX_10BIT)
        {
            print_syntax_error(Error_62, line->file_am_name, line->line_num);
            *errors_found = 1;
            return;
        }
        word |= BIT_ABSOLUTE_FLAG;
        temp = (unsigned short)(immediate_val & MASK_8BIT); /* 8-bit immediate value */
        word |= (temp << SHIFT_IMMEDIATE_VALUE); /* Bits 9-2 contain the immediate value */
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
        word |= BIT_MASK_SIGNAL; /* Signal for second pass to resolve */
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case DIRECT_REGISTER:
        /* Handle both direct and indirect register addressing */
        if (operand[0] == ASTERISK) {
            operand++; /* Skip the '*' for indirect addressing */
        }
        word |= BIT_ABSOLUTE_FLAG;
        word |= (which_regis(operand) << (operands_num == 1 ? SHIFT_DST_REGISTER : SHIFT_SRC_REGISTER));
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
        add_instruction_code(code, Usage, IC, BIT_MASK_SIGNAL, errors_found); /* Base address placeholder */
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

    word |= (ind << SHIFT_OPCODE_POS) | BIT_ABSOLUTE_FLAG;
    /* Addressing method is already in correct 2-bit format */
    dst_encoding = method;
    word |= (dst_encoding << SHIFT_DST_OPERAND);
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

    word |= (ind << SHIFT_OPCODE_POS) | BIT_ABSOLUTE_FLAG;
    /* Addressing methods are already in correct 2-bit format */
    dst_encoding = method_2;
    src_encoding = method;
    
    word |= (dst_encoding << SHIFT_DST_OPERAND);
    word |= (src_encoding << SHIFT_SRC_OPERAND);
    add_instruction_code(code, Usage, IC, word, errors_found);

    /* Combine if both operands are registers */
    if (method == DIRECT_REGISTER && method_2 == DIRECT_REGISTER)
    {
        /* Handle indirect addressing by skipping '*' if present */
        if (operand[0] == ASTERISK)
            operand++;
        if (second_operand[0] == ASTERISK)
            second_operand++;

        second_word |= BIT_ABSOLUTE_FLAG;
        second_word |= (which_regis(operand) << SHIFT_SRC_REGISTER);        /* First operand (r1) is source */
        second_word |= (which_regis(second_operand) << SHIFT_DST_REGISTER); /* Second operand (r4) is destination */
        add_instruction_code(code, Usage, IC, second_word, errors_found);
        return;
    }

    /* Handle separately */
    process_operation_code(code, Usage, IC, line, method, operand, opcodes[ind].operands_num, errors_found);
    process_operation_code(code, Usage, IC, line, method_2, second_operand, opcodes[ind].operands_num - 1, errors_found);
}
