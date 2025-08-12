/* This is the errors header file.*/
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/* Error struct definition */
typedef struct Error {
    int code_num;
    char *error_msg;
} Error;

typedef enum ERROR_CODES {
    Error_0 = 0, Error_1, Error_2,
    Error_4, Error_5,
    Error_6, Error_7, Error_8,
    Error_9, Error_10, Error_11,
    Error_13, Error_14,
    Error_15, Error_17,
    Error_18, Error_19, Error_20,
    Error_21, Error_22, Error_23,
    Error_24, Error_25, 
    Error_27, Error_28, Error_29,
    Error_30, Error_31, Error_32,
    Error_33, Error_34, Error_35,
    Error_36, Error_37, Error_38,
    Error_39, Error_40, Error_41,
    Error_42, Error_43, Error_44,
    Error_45, Error_46, Error_47,
    Error_48, Error_49, Error_50,
    Error_51, Error_52, Error_53,
    Error_54, Error_55, Error_56,
    Error_57, Error_58, Error_59,
    Error_60, Error_61, Error_62,
    Error_63, Error_64, Error_67, Error_68,
    Error_69, Error_70, 
    Error_72, Error_73, Error_74, 
    Error_75, Error_76
} ERROR_CODES;

/**
 * Prints a system error message based on the given error code.
 * @param error_code The code of the error to be printed.
 */
void log_system_error(int error_code);


/**
 * Prints a syntax error message including the file name and line number.
 * @param error_code The code of the error to be printed.
 * @param file_name The name of the file where the error occurred.
 * @param line_num The line number where the error occurred.
 */
void log_syntax_error(int error_code,char *file_name,int line_num);


/**
 * Prints a syntax error message including the file name, the line number and the specific error quoted.
 * @param error_code The code of the error to be printed.
 * @param file_name The name of the file where the error occurred.
 * @param line_num The line number where the error occurred.
 * @param word The word that caused the error.
 */

void log_unique_error(int error_code, char *file_name, int line_num, char *word);


#endif
