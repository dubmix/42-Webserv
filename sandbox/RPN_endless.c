#include <stdio.h>

int main() {
    int *ptr = NULL; // Initialize a pointer to NULL (pointing to nowhere)

    *ptr = 10; // Attempting to write to the memory location pointed by NULL

    return 0;
}