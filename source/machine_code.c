/**
 * This file handles the machine-coding for the assembler and contains functions for
 * handling operands and adding data and instruction codes to their arrays.
 */
#include <stdio.h>
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

/* ===== Helper for matrix operands ===== */
static void add_matrix_operands(unsigned short *code, int *Usage, int *IC, int address, int row_reg, int col_reg, int *errors_found)
{
    unsigned short base;
    unsigned short regs;

    /* First word: base address */
    base = ((address & 0xFF) << OPERAND_ADDR_SHIFT) | ARE_RELOCATABLE;
    add_instruction_code(code, Usage, IC, base, errors_found);

    /* Second word: row and column registers */
    regs = ((row_reg & 0xF) << MATRIX_ROW_SHIFT) |
           ((col_reg & 0xF) << MATRIX_COL_SHIFT) |
           ARE_ABSOLUTE;
    add_instruction_code(code, Usage, IC, regs, errors_found);
}

void process_operation_code(unsigned short *code, int *Usage, int *IC, Line *line, int method, char *operand, int operands_num, int *errors_found)
{
    unsigned short word = 0, temp = 0;
    char label_name[31];
    int row = 0, col = 0;

    switch (method)
    {
    case IMMEDIATE:
        operand++;
        word |= BIT_ABSOLUTE_FLAG;
        temp = (unsigned short)(atoi(operand) & MASK_10BIT);
        word |= (temp << SHIFT_DST_OPERAND);
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case DIRECT:
        if (add_label(operand, *IC, OPERAND, TBD) == NULL)
        {
            fclose(line->file);
            free_line(line);
            free_labels();
            free_macros();
            free_all_memory();
            exit(1);
        }
        word |= BIT_MASK_SIGNAL;
        word |= (line->line_num << BIT_MASK_SIGNAL);
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case INDIRECT_REGISTER:
        operand++;
        word |= BIT_ABSOLUTE_FLAG;
        word |= (which_regis(operand) << (operands_num == 1 ? SHIFT_DST_REGISTER : SHIFT_SRC_REGISTER));
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case DIRECT_REGISTER:
        word |= BIT_ABSOLUTE_FLAG;
        word |= (which_regis(operand) << (operands_num == 1 ? SHIFT_DST_REGISTER : SHIFT_SRC_REGISTER));
        add_instruction_code(code, Usage, IC, word, errors_found);
        return;

    case MATRIX:
        /* Expected syntax: LABEL[rX][rY] */
        if (sscanf(operand, "%30[^[][%d][%d]", label_name, &row, &col) == 3)
        {
            /* Add label reference for second pass resolution */
            if (add_label(label_name, *IC, OPERAND, TBD) == NULL)
            {
                fclose(line->file);
                free_line(line);
                free_labels();
                free_macros();
                free_all_memory();
                exit(1);
            }
            /* Generate two words: address placeholder + row/col regs */
            add_matrix_operands(code, Usage, IC, 0, row, col, errors_found);
        }
        else
        {
            print_syntax_error(Error_75, line->file_am_name, line->line_num);
        }
        return;

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

    word |= (ind << SHIFT_OPCODE_POS) | BIT_ABSOLUTE_FLAG;
    word |= (BIT_MASK << (method + SHIFT_DST_OPERAND));
    add_instruction_code(code, Usage, IC, word, errors_found);

    process_operation_code(code, Usage, IC, line, method, operand, opcodes[ind].operands_num, errors_found);
}

void handle_two_operands(unsigned short *code, int *Usage, int *IC, Line *line, char *operand, char *second_operand, int ind, int *errors_found)
{
    unsigned short word = 0, second_word = 0;
    Op_Code *opcodes = get_opcodes();
    int method, method_2;

    method = which_addressing_method(operand, line, errors_found);
    method_2 = which_addressing_method(second_operand, line, errors_found);

    word |= (ind << SHIFT_OPCODE_POS) | BIT_ABSOLUTE_FLAG;
    word |= (BIT_MASK << (method_2 + SHIFT_DST_OPERAND));
    word |= (BIT_MASK << (method + SHIFT_SRC_OPERAND));
    add_instruction_code(code, Usage, IC, word, errors_found);

    /* Combine if both operands are registers */
    if ((method == INDIRECT_REGISTER || method == DIRECT_REGISTER) &&
        (method_2 == INDIRECT_REGISTER || method_2 == DIRECT_REGISTER))
    {
        if (method == INDIRECT_REGISTER) operand++;
        if (method_2 == INDIRECT_REGISTER) second_operand++;

        second_word |= BIT_ABSOLUTE_FLAG;
        second_word |= (which_regis(operand) << SHIFT_SRC_REGISTER);
        second_word |= (which_regis(second_operand) << SHIFT_DST_REGISTER);
        add_instruction_code(code, Usage, IC, second_word, errors_found);
        return;
    }

    /* Handle separately */
    process_operation_code(code, Usage, IC, line, method, operand, opcodes[ind].operands_num, errors_found);
    process_operation_code(code, Usage, IC, line, method_2, second_operand, opcodes[ind].operands_num - 1, errors_found);
}
