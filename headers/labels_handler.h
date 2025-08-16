/**
 * This is the labels header file.
 * This file handles the labels of the program.
 */

#ifndef LABELS_HANDLER_H
#define LABELS_HANDLER_H

/* Type enum definition */
typedef enum Type {
    REGULAR,
    ENTRY,
    EXTERN,
    OPERAND
} Type;

/* Location enum definition */
typedef enum Location {
    TBD,  
    CODE,
    DATA
} Location;

/* Label struct definition */
typedef struct Label {
    char *name;
    int address;
    Type type;
    Location location;
    struct Label *next;
} Label;

/**
 * Adds a new Label to the end of the linked list.
 * name: The name of the new label.
 * address: The address of the new label in memory.
 * type: The type of the new label.
 * location: The location of the new label in the machine code.
 * return 0 for a successful operation ,1 if errors were detected.
 */
Label *add_label(char *name,int address,Type type,Location location);


/**
 * Checks if a given name is a label name.
 * label_name: The name to check.
 * return Pointer to the label if found, NULL otherwise.
 */
Label *is_label_name(char *label_name);


/**
 * Checks if a given label name is exist.
 * label_name: The name to check.
 * return Pointer to the label if found, NULL otherwise.
 */
Label *is_label_name_exist(char *label_name);


/**
 * Checks if all entry type labels exist.
 * file_am_name: The name of the file being processed.
 * return 1 if errors were found, 0 otherwise.
 */
int is_all_entry_labels_exist(char *file_am_name);


/**
 * Updates the addresses of data labels.
 * IC: Pointer to the instruction counter.
 */
void update_data_label(int *IC);


/**
 * Retrieves the first "operand" label.
 * return Pointer to the operand label if found, NULL otherwise.
 */
Label *get_first_operand();


/**
 * Checks if any "entry" type labels exist.
 * return 1 if an entry label exists, 0 otherwise.
 */
int is_entry_exist();


/**
 * Checks if any "extern" type labels exist.
 * return 1 if an extern label exists, 0 otherwise.
 */
int is_extern_exist();


/**
 * points to the head of the label list.
 * return Pointer to the head of the label list.
 */
Label *point_label_head();


/**
 * points to the last label in the linked list.
 * return Pointer to the last label in the list.
 */
Label *point_last_label();


/**
 * Removes the last label in the linked list.
 */
void remove_last_label();


/**
 * Removes a specific label from the linked list.
 * label: Pointer to the label to be removed.
 */
void remove_label(Label *label);


/**
 * Frees all the labels in the linked list.
 */
void free_labels();


#endif
