// Pointer safety test - demonstrates null pointer and uninitialized pointer issues

void null_pointer_test() {
    int x;
    int y;
    int* ptr;

    x = 10;
    ptr = 0;  // NULL assignment

    // VULNERABLE: Dereferencing NULL pointer
    y = *ptr;
}

void uninitialized_pointer() {
    int* data;
    int value;

    // VULNERABLE: Using uninitialized pointer
    value = *data;
}

int main() {
    null_pointer_test();
    uninitialized_pointer();
    return 0;
}
