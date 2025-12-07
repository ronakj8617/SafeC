// Integer overflow examples
#include <stdlib.h>

void overflow_in_malloc() {
    int count = 1000000000;
    int size = 1000;

    // VULNERABLE: Multiplication can overflow
    int* data = (int*)malloc(count * size);
    free(data);
}

void arithmetic_overflow() {
    int a = 2147483647;  // MAX_INT
    int b = 1;

    // VULNERABLE: Addition will overflow
    int result = a + b;
}

int main() {
    overflow_in_malloc();
    arithmetic_overflow();
    return 0;
}
