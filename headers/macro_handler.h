/**
 * This is the macros header file.
 * This file handles the macros of the program.
 * It includes functions for adding, checking, and removing macros.
 */
#ifndef MACRO_HANDLER_H
#define MACRO_HANDLER_H

/* Macro struct definition */
typedef struct Macro {
    char *name;
    char *content;
    int line;
    struct Macro *next;
} Macro;

/**
 * Adds a new macro to the end of the linked list.
 * name: The name of the new macro.
 * line: The line number associated with the new macro.
 * return 0 for a successful operation ,1 if errors were detected.
 */
int add_macro(char *name,int line);


/**
 * Checks if the given name is a macro name.
 * @macro_name: The name to check.
 * @return: Pointer to the macro if found, NULL otherwise.
 */
Macro *find_macro_by_name(char *macro_name);


/**
 * Changes the content of the last macro in the linked list.
 * @new_content: The content to append.
 * @return: 0 for a successful operation, 1 if errors were detected.
 */
int change_macro_content(char *new_content);


/**
 * Points to the last macro in the linked list.
 * @return Pointer to the last macro.
 */
Macro *point_last_macro();


/**
 * Removes the last macro from the linked list.
 */
void remove_last_macro();


/**
 * Frees all macros in the linked list.
 */
void free_macros();


#endif
