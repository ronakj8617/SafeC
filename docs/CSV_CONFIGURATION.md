# CSV Configuration Feature

## Overview

SafeC now supports **CSV-based configuration** for vulnerability detection rules! This means you can customize which functions are flagged as vulnerable **without recompiling the code**.

## What's New

### 📁 Configuration Files

Three new CSV files in the `config/` directory:

1. **`unsafe_functions.csv`** - Buffer overflow detection rules
2. **`format_functions.csv`** - Format string vulnerability rules  
3. **`memory_functions.csv`** - Memory management function tracking

### 🔧 How It Works

When SafeC runs, it automatically loads these CSV files to determine which functions to flag. If the files are missing, it falls back to built-in defaults.

## Usage Examples

### Example 1: Add a Custom Unsafe Function

Edit `config/unsafe_functions.csv`:

```csv
# Add your custom function
my_unsafe_copy,HIGH,my_safe_copy,Custom function without bounds checking
```

Save and run SafeC - no recompilation needed!

### Example 2: Change Severity Level

```csv
# Change strcpy from HIGH to CRITICAL
strcpy,CRITICAL,strncpy,Copies string without bounds checking
```

### Example 3: Temporarily Disable a Rule

```csv
# Comment out to disable
# gets,CRITICAL,fgets,Reads input without size limit
```

## Configuration File Formats

### unsafe_functions.csv
```
function_name,severity,safe_alternative,description
```

**Example:**
```csv
strcpy,HIGH,strncpy,Copies string without bounds checking
strcat,HIGH,strncat,Concatenates strings without bounds checking
```

### format_functions.csv
```
function_name,severity,description
```

**Example:**
```csv
printf,HIGH,Can be exploited with user-controlled format strings
fprintf,HIGH,Can be exploited with user-controlled format strings
```

### memory_functions.csv
```
function_name,type,description
```

**Example:**
```csv
malloc,ALLOC,Allocates memory on heap
free,FREE,Deallocates memory
```

## Benefits

✅ **No Recompilation** - Modify rules instantly  
✅ **Easy Customization** - Simple CSV format  
✅ **Project-Specific Rules** - Different configs for different projects  
✅ **Version Control Friendly** - Track rule changes in git  
✅ **Extensible** - Add your own custom functions

## Technical Details

- **ConfigLoader Class**: Parses CSV files and provides data to detectors
- **Automatic Loading**: Detectors load config on first use
- **Fallback Defaults**: Built-in rules if CSV files are missing
- **Comment Support**: Lines starting with `#` are ignored

## For Graduate Applications

This feature demonstrates:
- **Software Engineering**: Separation of configuration from code
- **Extensibility**: Plugin-like architecture
- **Usability**: Easy customization for end users
- **Best Practices**: Configuration management

Perfect example of making research tools practical and user-friendly!
