#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VAVLib.h" 

int main(int arg_count, char* arg_vector[]) {
    int value_to_find = 0;

    if (arg_count < 2) {
        fprintf(stderr, "Error (you didn't write the method)\n");
        fprintf(stderr, "Available methods: 'bsearch_vav', 'bsearch_r_vav'\n");
        return 1;
    }

    char* search_method_name = arg_vector[1];

    if (arg_count > 2) {
        value_to_find = atoi(arg_vector[2]);
        if (value_to_find == 0 && strcmp(arg_vector[2], "0") != 0) {
            fprintf(stderr, "Error (The second argument '%s' is not a valid number)\n", arg_vector[2]);
            return 1;
        }
    }
    else {
        printf("Please enter an integer to search for: ");
        if (scanf_s("%d", &value_to_find) != 1) {
            fprintf(stderr, "Input error (An integer was expected)\n");
            return 1; 
        }
    }

    int found_at_index = -1;

    const int last_element_index = 1024 - 1;

    if (strcmp(search_method_name, "bsearch_vav") == 0) {
        found_at_index = bsearch_vav(array, last_element_index, value_to_find);
    }
    else if (strcmp(search_method_name, "bsearch_r_vav") == 0) {
        found_at_index = bsearch_r_vav(array, value_to_find, 0, last_element_index);
    }
    else {
        fprintf(stderr, "Error: Unknown search method for '%s'\n", search_method_name);
        fprintf(stderr, "Use 'bsearch_vav' or 'bsearch_r_vav'.\n");
        return 1; 
    }

    if (found_at_index == -1) {
        printf("Result (%s): %d value is not found in the dataset\n",
            search_method_name, value_to_find);
    }
    else {
        printf("Result (%s): %d value found at position (index) %d\n",
            search_method_name, value_to_find, found_at_index);
    }

    return 0;
}