// Simplified format string test
// This version works with our simplified parser

void format_vuln(char user_input) {
    // VULNERABLE: User-controlled format string
    printf(user_input);
}

void format_mismatch() {
    int x;
    x = 10;
    // VULNERABLE: Format specifier count mismatch
    printf(x);
}

int main() {
    char input;
    format_vuln(input);
    format_mismatch();
    return 0;
}
