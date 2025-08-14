#include <stdio.h>
#include "errors.h"


/*
Function is used in order to centralize all internal error messages in one place, using an error code system.
Instead of printing error strings directly throughout the program, function gets called with a number as an input and it prints the correct error message.
*/
void print_internal_error(int code) {
    switch (code) {
        case 7:
            printf("ERROR: Failed to open output file.\n");
            break;
        case 8:
            printf("ERROR: Failed to open input file.\n");
            break;
        case 9:
            printf("ERROR: Macros name is missing.\n");
            break;
        case 13:
            printf("ERROR: Macro is already defined.\n");
            break;
        default:
            printf("ERROR: Unknown internal error.\n");
    }
}
