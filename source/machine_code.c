/**
 * This file handles the machine-coding for the assembler and contains functions for
 * handling operands and adding data and instruction codes to their arrays.
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "machine_code.h"
#include "error_handler.h"
#include "validator.h"
#include "labels.h"
#include "macros.h"
#include "utils.h"
#include "definitions.h"

void add_data_code(unsigned short *data, int *DC, int number)
{
    /* Store as 10-bit 2's complement */
    unsigned short word = (unsigned short)(number & MASK_10_BITS);

    data[*DC] = word;
    (*DC)++;
}

void add_instruction_code(unsigned short *code, int *memory_usage, int *instruction_counter, unsigned short word, int *error_counter)
{
    if (*memory_usage == MAX_ARRAY_CAPACITY)
    {
        log_system_error(Error_73);
        *error_counter = 1;
        (*memory_usage)++;
        return;
    }
    if (*memory_usage > MAX_ARRAY_CAPACITY)
        return;

    code[*instruction_counter] = (word & MASK_10_BITS);
    (*instruction_counter)++;
    *memory_usage += 1;
}

void process_operation_code(unsigned short *code, int *memory_usage, int *instruction_counter, Line *context, int method, char *operand, int operand_count, int *error_counter)
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
            log_syntax_error(Error_62, context->file_am_name, context->line_num);
            *error_counter = 1;
            return;
        }
        word |= ARE_ABSOLUTE;
        temp = (unsigned short)(immediate_val & MASK_8_BITS); /* 8-bit immediate value */
        word |= (temp << IMMEDIATE_VALUE_SHIFT_POSITION); /* Bits 9-2 contain the immediate value */
        add_instruction_code(code, memory_usage, instruction_counter, word, error_counter);
        return;
    }

    case DIRECT:
        /* Store the operand label for second pass resolution */
        added_label = add_label(operand, *instruction_counter, OPERAND, TBD);
        if (added_label == NULL)
        {
            fclose(context->file);
            free_line(context);
            free_labels();
            free_macros();
            free_all_memory();
            exit(1);
        }
        /* Placeholder for second pass resolution - address will be filled in second pass */
        word |= ARE_PLACEHOLDER_SIGNAL; /* Signal for second pass to resolve */
        add_instruction_code(code, memory_usage, instruction_counter, word, error_counter);
        return;

    case DIRECT_REGISTER:
        /* Handle both direct and indirect register addressing */
        if (operand[0] == ASTERISK_SIGN) {
            operand++; /* Skip the '*' for indirect addressing */
        }
        word |= ARE_ABSOLUTE;
        word |= (parse_register_operand(operand) << (operand_count == 1 ? DESTINATION_REGISTER_SHIFT_POSITION : SOURCE_REGISTER_SHIFT_POSITION));
        add_instruction_code(code, memory_usage, instruction_counter, word, error_counter);
        return;

    case MATRIX:
    {
        /* Store the full matrix expression for second pass parsing */
        if (add_label(operand, *instruction_counter, OPERAND, TBD) == NULL)
        {
            log_system_error(Error_1);
            *error_counter = 1;
            return;
        }
        /* Matrix addressing: add placeholders for second pass resolution */
        add_instruction_code(code, memory_usage, instruction_counter, ARE_PLACEHOLDER_SIGNAL, error_counter); /* Base address placeholder */
        add_instruction_code(code, memory_usage, instruction_counter, 0, error_counter); /* Register combination placeholder */
        return;
    }

    default:
        /* Unknown addressing method */
        log_syntax_error(Error_69, context->file_am_name, context->line_num);
        *error_counter = 1;
        return;
    }
}

void handle_one_operand(unsigned short *code, int *memory_usage, int *instruction_counter, Line *context, int method, char *operand, int ind, int *error_counter)
{
    unsigned short word = 0;
    InstructionDefinition *opcodes = retrieve_instruction_set();
    int dst_encoding;

    word |= (ind << OPCODE_SHIFT_POSITION) | ARE_ABSOLUTE;
    /* Addressing method is already in correct 2-bit format */
    dst_encoding = method;
    word |= (dst_encoding << DESTINATION_OPERAND_SHIFT_POSITION);
    add_instruction_code(code, memory_usage, instruction_counter, word, error_counter);

    process_operation_code(code, memory_usage, instruction_counter, context, method, operand, opcodes[ind].operand_count, error_counter);
}

void handle_two_operands(unsigned short *code, int *memory_usage, int *instruction_counter, Line *context, char *operand, char *second_operand, int ind, int *error_counter)
{
    unsigned short word = 0, second_word = 0;
    InstructionDefinition *opcodes = retrieve_instruction_set();
    int method, method_2;
    int dst_encoding, src_encoding;

    method = determine_operand_addressing_mode(operand, context, error_counter);
    method_2 = determine_operand_addressing_mode(second_operand, context, error_counter);

    word |= (ind << OPCODE_SHIFT_POSITION) | ARE_ABSOLUTE;
    /* Addressing methods are already in correct 2-bit format */
    dst_encoding = method_2;
    src_encoding = method;
    
    word |= (dst_encoding << DESTINATION_OPERAND_SHIFT_POSITION);
    word |= (src_encoding << SOURCE_OPERAND_SHIFT_POSITION);
    add_instruction_code(code, memory_usage, instruction_counter, word, error_counter);

    /* Combine if both operands are registers */
    if (method == DIRECT_REGISTER && method_2 == DIRECT_REGISTER)
    {
        /* Handle indirect addressing by skipping '*' if present */
        if (operand[0] == ASTERISK_SIGN)
            operand++;
        if (second_operand[0] == ASTERISK_SIGN)
            second_operand++;

        second_word |= ARE_ABSOLUTE;
        second_word |= (parse_register_operand(operand) << SOURCE_REGISTER_SHIFT_POSITION);        /* First operand (r1) is source */
        second_word |= (parse_register_operand(second_operand) << DESTINATION_REGISTER_SHIFT_POSITION); /* Second operand (r4) is destination */
        add_instruction_code(code, memory_usage, instruction_counter, second_word, error_counter);
        return;
    }

    /* Handle separately */
    process_operation_code(code, memory_usage, instruction_counter, context, method, operand, opcodes[ind].operand_count, error_counter);
    process_operation_code(code, memory_usage, instruction_counter, context, method_2, second_operand, opcodes[ind].operand_count - 1, error_counter);
}
