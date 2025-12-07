// Simplified buffer overflow test - no includes
// This version works with our simplified parser

#include <string.h>
void unsafe_copy() {
    char buffer[10];
    char input[100];

    // VULNERABLE: strcpy without bounds checking
    strcpy(buffer, input);
}

void array_overflow() {
    int arr[5];

    // VULNERABLE: Out of bounds access
    arr[10] = 42;
}

int main() {
    unsafe_copy();
    return 0;
}
