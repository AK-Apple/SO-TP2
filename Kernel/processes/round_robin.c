#include "IO.h"
#include "lib.h"
#include "round_robin.h"

// Function to initialize the list
void init_list(CircularList *list)
{
    list->current_index = 0;
    list->size = 0;
}

// Function to get the next element and move the current index
int next(CircularList *list)
{
    if (list->size == 0)
    {
        return -1;
    }
    int current_value = list->array[list->current_index];
    list->current_index = (list->current_index + 1) % list->size;
    return current_value;
}

// Function to delete all occurrences of a given value
void delete_value(CircularList *list, int value)
{
    int i, new_size = 0, new_current_index = -1;

    // Shift elements to remove the given value
    for (i = 0; i < list->size; i++)
    {
        if (list->array[i] != value)
        {
            list->array[new_size] = list->array[i];
            if (i == list->current_index)
            {
                new_current_index = new_size; 
            }
            new_size++;
        }
    }

    // Update list properties
    list->size = new_size;
    if (new_size == 0)
    {
        list->current_index = 0; 
    }
    else if (new_current_index != -1)
    {
        list->current_index = new_current_index;
    }
    else if (list->current_index >= list->size)
    {
        list->current_index = 0; 
    }
}

// Function to add a new item at the end
void add(CircularList *list, int value)
{
    if (list->size < MAX_SIZE)
    {
        list->array[list->size] = value;
        list->size++;
    }
    else
    {
        printf("List is full, cannot add more items\n");
    }
}

void delete_value_ocurrence(CircularList *list, int value)
{
    if (list->size == 0)
    {
        return; 
    }

    int i, found_index = -1;

    // Find the first occurrence of the value
    for (i = 0; i < list->size; i++)
    {
        if (list->array[i] == value)
        {
            found_index = i;
            break; 
        }
    }

    if (found_index != -1)
    {
        // Swap with the last element
        list->array[found_index] = list->array[list->size - 1];

        list->size--;

        if (found_index < list->current_index)
        {
            list->current_index--;
        }
        else if (found_index == list->current_index)
        {
            if (list->size == 0)
            {
                list->current_index = 0; 
            }
            else
            {
                list->current_index = found_index % list->size; 
            }
        }
    }
}


void display_list(CircularList *list) 
{
    putChar('[');
    for(int i = 0; i < list->size; i++) 
    {
        if(list->current_index == i) putChar('(');
        k_print_int_dec(list->array[i]);
        if(list->current_index == i) putChar(')');
        printf(", ");
    }
    printf("]\n");
}
