// Use-after-free examples
#include <stdio.h>
#include <stdlib.h>

void use_after_free_simple() {
    int* ptr = (int*)malloc(sizeof(int));
    *ptr = 42;

    free(ptr);

    // VULNERABLE: Using pointer after free
    printf("%d\n", *ptr);
}

void double_free() {
    int* ptr = (int*)malloc(sizeof(int));

    free(ptr);
    // VULNERABLE: Double free
    free(ptr);
}

int main() {
    use_after_free_simple();
    return 0;
}
