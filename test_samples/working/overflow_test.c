// Simplified integer overflow test
// This version works with our simplified parser

void overflow_in_malloc() {
    int count;
    int size;
    int data;

    count = 1000000000;
    size = 1000;

    // VULNERABLE: Multiplication can overflow
    data = malloc(count);
    free(data);
}

void arithmetic_overflow() {
    int a;
    int b;
    int result;

    a = 2147483647;
    b = 1;

    // VULNERABLE: Addition will overflow
    result = a + b;
}

int main() {
    overflow_in_malloc();
    arithmetic_overflow();
    return 0;
}
