# SafeC Configuration Files

This directory contains CSV configuration files that define the rules for vulnerability detection. You can modify these files to customize SafeC's behavior without recompiling.

## Configuration Files

### 1. `unsafe_functions.csv`
Defines functions that can cause buffer overflows.

**Format**: `function_name,severity,safe_alternative,description`

**Example**:
```csv
strcpy,HIGH,strncpy,Copies string without bounds checking
```

**Severity Levels**: LOW, MEDIUM, HIGH, CRITICAL

### 2. `format_functions.csv`
Defines functions vulnerable to format string attacks.

**Format**: `function_name,severity,description`

**Example**:
```csv
printf,HIGH,Can be exploited with user-controlled format strings
```

### 3. `memory_functions.csv`
Defines memory allocation and deallocation functions.

**Format**: `function_name,type,description`

**Types**: ALLOC (allocation), FREE (deallocation)

**Example**:
```csv
malloc,ALLOC,Allocates memory on heap
free,FREE,Deallocates memory
```

## How to Modify

1. **Add a new unsafe function**:
   - Open `unsafe_functions.csv`
   - Add a new line: `function_name,severity,safe_alternative,description`
   - Save the file

2. **Change severity levels**:
   - Edit the severity column (LOW, MEDIUM, HIGH, CRITICAL)
   - Save the file

3. **Remove a function from detection**:
   - Comment out the line with `#` or delete it
   - Save the file

## Notes

- Lines starting with `#` are comments and will be ignored
- Empty lines are ignored
- Changes take effect immediately (no recompilation needed)
- If a config file is missing, SafeC will use built-in defaults

## Example Customization

To add detection for a custom unsafe function:

```csv
# Add to unsafe_functions.csv
my_unsafe_copy,HIGH,my_safe_copy,Custom function without bounds checking
```

To disable detection of a specific function, comment it out:

```csv
# strcpy,HIGH,strncpy,Copies string without bounds checking
```
