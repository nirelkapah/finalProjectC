#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*making sure memory allocation using malloc doesn't fail (means returns null)*/
void *handle_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        printf("ERROR: Memory allocation failed.\n");
        exit(1);
    }
    return ptr;
}
