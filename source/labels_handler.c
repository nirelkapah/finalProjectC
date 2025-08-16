/**
 * This is the labels handling file of the assembler that includes functions
 * to add, search, and manage labels in a linked list, and also to update addresses and free the allocated memory.
 * It handles different types of labels such as regular, entry, or external.
 * This file defines a static head variable to simplify memory management and error handling and avoiding the need to pass head as a parameter
 * to functions, allowing easy access to the labels linked list for cleanup in case of errors, while keeping the variable encapsulated within the file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "labels_handler.h"
#include "error_handler.h"
#include "definitions.h"

/* Defining the head of the labels linked list */

static Label *head = NULL;

Label *add_label(char *name, int address, Type type, Location location)
{
    Label *last_label;
    Label *new_label;

    /* Allocate memory for new label */
    new_label = (Label *)malloc(sizeof(Label));
    if (new_label == NULL)
    {
        log_system_error(Error_101);
        return NULL; /* Indicates failure */
    }

    /* Allocating memory and copying the name */
    new_label->name = (char *)malloc(strlen(name) + 1); /* +1 to accommodate '\0' */
    if (new_label->name == NULL)
    {
        log_system_error(Error_101);
        free(new_label);
        return NULL; /* Indicates failure */
    }
    strcpy(new_label->name, name);



    /* Setting the address, type, location and next pointer */
    new_label->address = address;
    new_label->type = type;

    if (location != TBD)
    {
        new_label->location = location;
    }
    else
    {
        new_label->location = TBD;
    }
    new_label->next = NULL;

    /* If the list is empty, setting the new label as the head */
    if (head == NULL)
    {
        head = new_label;
    }
    else
    {
        /* Otherwise, finding the end of the list and adding the new label */
        last_label = point_last_label();
        last_label->next = new_label;
    }
    return new_label; /* Indicates success */
}

Label *is_label_name(char *label_name)
{
    Label *current = head;
    while (current != NULL)
    {
        if (strcmp(current->name, label_name) == 0)
        {
            return current; /* return even if OPERAND type */
        }
        current = current->next;
    }
    return NULL;
}

Label *is_label_name_exist(char *label_name)
{
    Label *current = head;

    while (current != NULL)
    {
        if (strcmp(current->name, label_name) == 0 && /* Determining if label was defined based on location and type */
            ((current->type != EXTERN && current->location != TBD) || (current->type == EXTERN && current->location == TBD)))
        {
            return current; /* Indicates name is a label name and returns a pointer to its node */
        }
        current = current->next;
    }
    return NULL; /* Indicates name is not a label name */
}

int is_all_entry_labels_exist(char *file_am_name)
{
    Label *current = head;
    int errors_found = 0;

    while (current != NULL)
    {
        if (current->type == ENTRY && current->location == TBD)
        { /* Checking for an undefined "entry" label */
            printf(" Undefined reference detected in File \"%s\" - Label \"%s\"", file_am_name, current->name);
            log_system_error(Error_263);
            errors_found = 1; /* Indicates not all "entry" labels were defined */
        }
        current = current->next;
    }
    return errors_found;
}

void update_data_label(int *IC)
{
    Label *current = head;

    while (current != NULL)
    {
        if (current->location == DATA)
        {
            current->address += *IC + MEMORY_START_ADDRESS;
        }
        current = current->next;
    }
}

Label *get_first_operand()
{
    Label *current = head;

    while (current != NULL)
    {
        if (current->type == OPERAND)
        {
            return current; /* Indicates an "operand" type label was found */
        }
        current = current->next;
    }
    return NULL; /* Indicates no "operand" type label was found */
}

int is_entry_exist()
{
    Label *current = head;

    while (current != NULL)
    {
        if (current->type == ENTRY)
        {
            return 1; /* Indicates an "entry" type label was found */
        }
        current = current->next;
    }
    return 0; /* Indicates no "entry" type label was found */
}

int is_extern_exist()
{
    Label *current = head;

    while (current != NULL)
    {
        if (current->type == EXTERN)
        {
            return 1; /* Indicates an "extern" type label was found */
        }
        current = current->next;
    }
    return 0; /* Indicates no "extern" type label was found */
}

Label *point_label_head()
{
    return head;
}

Label *point_last_label()
{
    Label *current;

    if (head == NULL)
        return NULL; /* Indicates list is empty */

    current = head;
    while (current->next != NULL)
        current = current->next;

    return current; /* Returning the last label in the list */
}

void remove_last_label()
{
    Label *current;

    if (head->next == NULL)
    { /* Indicates only one label in the list */
        free(head->name);
        free(head);
        head = NULL;
        return;
    }
    current = head;

    while (current->next->next != NULL)
    { /* Skipping to the second-to-last label */
        current = current->next;
    }
    free(current->next->name);
    free(current->next);

    current->next = NULL; /* Updating the second-to-last label to be the new last */
}

void remove_label(Label *label)
{
    Label *current = head;
    Label *prev = NULL;

    while (current != NULL)
    {
        if (current == label)
        {
            if (prev == NULL)
            { /* Checking if "head" is the label to be removed */
                head = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current->name);
            free(current);
            return; /* Label found and removed */
        }
        prev = current;
        current = current->next;
    }
}

void free_labels()
{
    Label *current = head;
    Label *next;

    while (current != NULL)
    {
        next = current->next; /* Updating the next pointer */

        free(current->name); /* Freeing the dynamically allocated name */
        free(current);       /* Freeing the macro node itself */

        current = next; /* Moving to the next node */
    }
    head = NULL;
}
