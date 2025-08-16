/**
 * This is the second pass header file.
 * Explanation of the process:
 * This is final step in the assembly process.
 * Instead of reading line by line again, this file scans the instruction array for uncoded label addresses marked by the first pass.
 * For each uncoded label, it retrieves the address from the corresponding label operand that was identified in the first pass.
 * If the operand label was defined during the first pass, its address is retrieved and updated in the instruction array.
 * This approach is efficient, as it avoids redundant file reading and directly addresses only the necessary updates.
 */

#ifndef ASSEMBLER_SECOND_PASS_H
#define ASSEMBLER_SECOND_PASS_H

/**
 * This function performs the second pass of the assembler.
 * file_am_name: The name of the input file after pre-processing.
 * code: Array containing the instruction code.
 * data: Array containing the data code.
 * ic: Pointer to the instruction counter.
 * dc: Pointer to the data counter.
 * return 0 for a successful operation, 1 if errors were detected.
 */
int second_pass(char *file_am_name,unsigned short *code,unsigned short *data,int *ic,int *dc);


/**
 * Codes the uncoded operand labels in the instruction code.
 * file_am_name: The name of the input file after pre-processing.
 * code: Array containing the instruction code.
 * ic: Pointer to the instruction counter.
 * return 0 if no errors were detected, 1 if errors were detected.
 */
int code_operand_labels(char *file_am_name,unsigned short *code,int *ic);


#endif
