// Simple test for SafeC analyzer
int main() {
    char buffer[10];
    int arr[5];

    // This will trigger buffer overflow detection
    arr[10] = 42;

    return 0;
}
