// Memory leak examples
#include <stdlib.h>

void simple_leak() {
    int* ptr = (int*)malloc(sizeof(int) * 100);
    *ptr = 42;

    // VULNERABLE: Memory allocated but never freed
    return;
}

void leak_in_loop() {
    for (int i = 0; i < 10; i++) {
        int* data = (int*)malloc(sizeof(int) * 50);
        // VULNERABLE: Memory allocated in loop but never freed
    }
}

int main() {
    simple_leak();
    leak_in_loop();
    return 0;
}
