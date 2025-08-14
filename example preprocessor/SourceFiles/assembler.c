#include <stdio.h>
#include <stdlib.h>
#include "preproc.h"

/*
argc - number of words executed in the terminal command when the first one is running the assembler.c
argv - array of the words executed in the terminal command
these parameteres are given to access given file names, and in order to check that the given command meets the standarts.
*/
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Missing file name (input).\n");
        return 1;
    }

    while (--argc > 0) {
        printf("Processing the following file: %s.as\n", argv[argc]);

        if (!mcro_exec(argv[argc])) {
            printf("Macro expansion failed for %s.as. Skipping the file.\n", argv[argc]);
            continue;  
        }

        printf("Macro expansion completed for the file: %s\n", argv[argc]);
    }

    return 0;
}

