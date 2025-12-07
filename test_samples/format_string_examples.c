// Format string vulnerabilities
#include <stdio.h>

void format_string_vuln(char* user_input) {
    // VULNERABLE: User-controlled format string
    printf(user_input);
}

void format_mismatch() {
    int x = 10;
    // VULNERABLE: Format specifier count mismatch
    printf("%d %d %d\n", x);
}

int main() {
    char input[] = "%x %x %x %x";
    format_string_vuln(input);
    format_mismatch();
    return 0;
}
