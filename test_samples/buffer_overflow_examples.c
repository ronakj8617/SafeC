// Buffer overflow examples
#include <stdio.h>
#include <string.h>

void unsafe_copy() {
    char buffer[10];
    char input[100] = "This is a very long string that will overflow";

    // VULNERABLE: strcpy without bounds checking
    strcpy(buffer, input);
}

void unsafe_gets() {
    char buffer[50];

    // VULNERABLE: gets() is inherently unsafe
    gets(buffer);
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
