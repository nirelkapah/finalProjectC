#include <stdio.h>
#include "error_handler.h"
#include "utils.h"
#include "pre_processor.h"
#include "assembler_first_pass.h"
#include "definitions.h"

/**
 * This is the main function that receives assembly input files (written in a specific language defined by the project's requirements).
 * The function then passes them over to the analysis of the "Three Steps Assembler".
 * argc: The number of command-line arguments.
 * argv: An array of strings containing the command-line arguments.
 * return Returns 0 on successful completion.
 */

int main(int argc, char *argv[]) {
    int i = 1;
    FILE *file;
    char *file_name;

    if (argc < BINARY_BASE) {  /* Checking if no files were entered */
        log_system_error(Error_100);
        return 1;  /* Indicates faliure */
    }
    /* Scanning files */
    for (; i < argc; i++) {
        file_name = valid_file_name(argv[i]);  /* Validating the input file name */
        if (file_name == NULL)
            continue;  /* Next file */

        file = search_file(file_name);
        if (file == NULL)
            continue;  /* Next file */

        printf("\nInitializing assembly process for: \"%s\"\n",file_name);

        /* Starting run_pre_processing */
        if (run_pre_processing(file_name) != 0) {
            printf("Assembly operation halted due to preprocessing issues\n");
            continue;  /* Skipping to the next file */
        }
        /* Starting first pass */
        if (run_first_pass(file_name) != 0) {
            printf("Assembly compilation aborted\n");
            continue;  /* Skipping to the next file */
        }
        printf("Assembly compilation completed successfully \n");
        free_all_memory();
    }
    return 0;  /* Success */
}
