#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h" /* for using func - handle_malloc */
#include "errors.h" /* for using func - print_internal_error */
#include "data_general_strct.h"
/* CONSTANTS */
#define MAX_LINE_LENGTH 81
#define BIG_NUMBER_CONST 1000


/* --- FUNCTION DECLARATIONS --- */
void add_to_list(node **head, char *name, char *content, int line_num);
node *search_list(node *head, char *name, int *found);
char *add_new_file(char *filename);
void print_internal_error(int code);

/* --- IMPLEMENTATION --- */

/*
Function creats a new node (which contains macro) and inserts the macros name and content into it
Afterwards, we update the node head accordingly.
*/
void add_to_list(node **head, char *name, char *content, int line_num){
    node *new_node = handle_malloc(sizeof(node));
    new_node->name = handle_malloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    new_node->content = handle_malloc(strlen(content) + 1);
    strcpy(new_node->content, content);
    new_node->line = line_num;
    new_node->next = *head;
    *head = new_node;
}

/*
Function searches for the wanted macro in the list, if found returns it
if not, keeps searching.
At the end, if the wanted macro wasn't found, function returns null.
*/
node *search_list(node *head, char *name, int *found) {
    while (head) {
        if (strcmp(head->name, name) == 0) {
            *found = 1;
            return head;
        }
        head = head->next;
    }
    *found = 0;
    return NULL;
}


/*
Function creates a new string that consists of 
the wanted file name @filename
+ the extention .am instead of .as
*/
char *add_new_file(char *filename)
{
    const char *NEW_EXT = ".am";          
    size_t len_ext = strlen(NEW_EXT);
    size_t len_fname;   
    char *new_name;        
    
    const char *dot = strrchr(filename, '.');


    if (dot && strcmp(dot, ".as") == 0) {
        size_t base_len = dot - filename;             
        char *new_name  = handle_malloc(base_len + len_ext + 1);

        strncpy(new_name, filename, base_len);       
        new_name[base_len] = '\0';
        strcat(new_name, NEW_EXT);                    
        return new_name;
    }

  
    len_fname = strlen(filename);
    new_name = handle_malloc(len_fname + len_ext + 1);
    strcpy(new_name, filename);
    strcat(new_name, NEW_EXT);
    return new_name;
}

int mcro_exec(char *file_name) {
    FILE *in_file, *out_file;
    char line[MAX_LINE_LENGTH];
    char macro_text[BIG_NUMBER_CONST];
    char *macro_name, *new_file, *in_name;
    node *head = NULL;
    int inside_macro = 0;
    int line_num = 0;
    int found ;

    new_file = add_new_file(file_name);
    in_name = handle_malloc(strlen(file_name) + 4);
    sprintf(in_name, "%s.as", file_name);

    in_file = fopen(in_name, "r");
    if (!in_file) {
        print_internal_error(8);  /* failed to open the new file (input) */
        free(in_name);
        return 0;
    }

    out_file = fopen(new_file, "w");
    if (!out_file) {
        print_internal_error(7);  /* failed to open output */
        fclose(in_file);
        return 0;
    }

    while (fgets(line, MAX_LINE_LENGTH, in_file)) {
        char temp[MAX_LINE_LENGTH];
        char *first_word;
        node *result;
        line_num++;

        /* Start of macro */
        if (!inside_macro && strncmp(line, "mcro", 4) == 0) {
            macro_name = strtok(line + 4, " \t\n");
            if (!macro_name) {
                print_internal_error(9);  /* macro name is missing*/
                fclose(in_file);
                fclose(out_file);
                free(new_file);
                return 0;
            }

            search_list(head, macro_name, &found);
            if (found) {
                print_internal_error(13);  /* The macro is already defined*/
                fclose(in_file);
                fclose(out_file);
                free(new_file);
                return 0;
            }

            inside_macro = 1;
            macro_text[0] = '\0';  /* Clears previous macro content*/
            continue;
        }

        /* Inside macro - collects lines until encouters "endmcro"*/
        if (inside_macro) {
            if (strncmp(line, "endmcro", 7) == 0) {
                inside_macro = 0;
                add_to_list(&head, macro_name, macro_text, line_num);
            } else {
                strcat(macro_text, line);  
            }
            continue;
        }

        /* Outside macro check */
        strcpy(temp, line);
        first_word = strtok(temp, " \t\n");

       
        if (first_word == NULL) {         
            fputs(line, out_file);        
         continue;                     
        }

        result = search_list(head, first_word, &found);
        if (found) {
            fputs(result->content, out_file);  /* write expanded macro */
        } else {
            fputs(line, out_file);  /* write the original line */
        }
    }
    

    fclose(in_file);
    fclose(out_file);
    free(new_file);
    free(in_name); 
    return 1;

}








