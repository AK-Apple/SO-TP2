#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

#define MAX_SIZE 128  // Maximum size of the circular array

// Circular bounded array structure definition
typedef struct {
    int array[MAX_SIZE];   // Circular array to hold integers
    int current_index;     // Index of the current element
    int size;              // Current number of elements in the list
} CircularList;

// Initializes the circular list, setting the current index and size to 0
void init_list(CircularList* list);

// Returns the current element and moves the current index to the next element
int next(CircularList* list);

// Deletes all occurrences of a specified value in the list. Adjusts current index if needed
void delete_value(CircularList* list, int value);

// Adds a new item at the end of the list. If the list is full, it prints an error
void add(CircularList* list, int value);

// Displays the current state of the list (used for debugging)
void display_list(CircularList* list);

// TODO: falta la función 

#endif // CIRCULAR_LIST_H
