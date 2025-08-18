/**
 * This is the errors header file.
 * This file handles the error handling of the program.
 */
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/* Error struct definition */
typedef struct Error {
    int num;
    char *message;
} Error;

typedef enum ERROR_CODES {
    /* 100-199: System errors */
    Error_100 = 100, Error_101, Error_102, Error_103, Error_104, Error_105,
    /* 200-299: Syntax/semantic errors */
    Error_200 = 200, Error_201, Error_202, Error_203, Error_204, Error_205,
    Error_206, Error_207, Error_208, Error_209, Error_210, Error_211,
    Error_212, Error_213, Error_214, Error_215, Error_216, Error_217,
    Error_218, Error_219, Error_220, Error_221, Error_222, Error_223,
    Error_224, Error_225, Error_226, Error_227, Error_228, Error_229,
    Error_230, Error_231, Error_232, Error_233, Error_234, Error_235,
    Error_236, Error_237, Error_238, Error_239, Error_240, Error_241,
    Error_242, Error_243, Error_244, Error_245, Error_246, Error_247,
    Error_248, Error_249, Error_250, Error_251, Error_252, Error_253,
    Error_254, Error_255, Error_256, Error_257, Error_258, Error_259,
    Error_260, Error_261, Error_262, Error_263
} ERROR_CODES;

/**
 * Prints a system error message based on the given error code.
 * @error_code: The code of the error to be printed.
 */
void log_system_error(int error_code);


/**
 * Prints a syntax error message including the file name and line number.
 * @error_code: The code of the error to be printed.
 * @file_name: The name of the file where the error occurred.
 * @line_num: The line number where the error occurred.
 */
void log_syntax_error(int error_code,char *file_name,int line_num);


#endif
