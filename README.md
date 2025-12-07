# SafeC: Static Analyzer for C/C++ Vulnerability Detection

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-064F8C.svg)](https://cmake.org/)

A research-oriented static analysis tool designed to detect common security vulnerabilities in C/C++ code. SafeC demonstrates advanced program analysis techniques suitable for cybersecurity research and practical vulnerability detection.

## 🎯 Project Overview

SafeC is a static analyzer built from the ground up to showcase understanding## ✨ Key Features

### Advanced Analysis Capabilities
- **Custom Lexer & Parser**: Built from scratch to demonstrate compiler construction principles
- **Abstract Syntax Tree (AST)**: Complete node hierarchy with visitor pattern implementation
- **Six Vulnerability Detectors**: Comprehensive coverage of CWE Top 25 vulnerability classes
- **Pointer Type Support**: Parser handles pointer declarations (`int*`, `char*`, etc.)
- **Dataflow Analysis**: Tracks variable states and memory through program execution
- **Control Flow Analysis**: Monitors execution paths and conditional branches

### CSV-Based Configuration System
- **115+ Configurable Detection Rules**: Extensive vulnerability pattern database
- **No Recompilation Required**: Modify detection rules on-the-fly
- **Project-Specific Customization**: Add custom unsafe functions for your codebase
- **Three Configuration Files**:
  - `config/unsafe_functions.csv` - 36+ buffer overflow patterns
  - `config/format_functions.csv` - 29+ format string vulnerabilities
  - `config/memory_functions.csv` - 28+ memory management functions

### Professional Output
- **Console Reports**: Color-coded severity levels with detailed recommendations
- **JSON Export**: Machine-readable output for CI/CD integration
- **Line-Precise Locations**: Exact file, line, and column information for each finding
- **Severity Classification**: CRITICAL, HIGH, MEDIUM, LOW severity levels with 115+ configurable patterns

This project was developed to demonstrate research capabilities in cybersecurity, specifically in the areas of program analysis and vulnerability detection.

## 🔍 Detected Vulnerability Classes

SafeC implements **six critical vulnerability detectors**:

### 1. Buffer Overflow Detection
- Identifies unsafe functions (`strcpy`, `strcat`, `sprintf`, `gets`, and 36+ more)
- Analyzes array bounds in indexing operations
- Tracks buffer sizes through dataflow analysis
- **CSV Configurable**: 36+ unsafe functions in `config/unsafe_functions.csv`
- **Severity**: HIGH to CRITICAL

### 2. Use-After-Free Detection
- Tracks memory allocation and deallocation (`malloc`, `free`, `new`, `delete`)
- Monitors pointer lifetime across scopes
- Detects pointer dereferences after `free`/`delete`
- Identifies double-free vulnerabilities
- **Severity**: CRITICAL

### 3. Memory Leak Detection
- Identifies allocated memory without corresponding deallocation
- Tracks memory through function calls and returns
- Detects leaks in error handling paths
- **CSV Configurable**: 28+ memory management functions in `config/memory_functions.csv`
- **Severity**: MEDIUM

### 4. Format String Vulnerabilities
- Detects user-controlled format strings in `printf` family functions
- Validates format specifier count vs. argument count
- Identifies dangerous `%n` specifiers
- **CSV Configurable**: 29+ format functions in `config/format_functions.csv`
- **Severity**: HIGH to CRITICAL

### 5. Integer Overflow Detection
- Analyzes arithmetic operations for potential overflows
- Checks array index calculations
- Detects overflow in memory allocation size calculations
- **Severity**: MEDIUM to HIGH

### 6. Pointer Safety Detection (NEW!)
- **Null Pointer Dereference**: Detects when NULL pointers are dereferenced without checks
- **Uninitialized Pointers**: Finds pointers used before initialization
- **Pointer State Tracking**: Monitors pointer lifecycle (NULL, VALID, FREED, UNINITIALIZED)
- **Pointer Arithmetic**: Warns about potentially dangerous pointer arithmetic
- **Severity**: HIGH to CRITICAL
- Detects overflow in memory allocation size calculations
- **Severity**: MEDIUM to HIGH

## 🏗️ Architecture

```
SafeC/
├── include/              # Header files
│   ├── lexer.h          # Tokenization
│   ├── parser.h         # Syntax analysis
│   ├── ast.h            # Abstract Syntax Tree
│   └── detectors/       # Vulnerability detectors
├── src/                 # Implementation
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── ast.cpp
│   ├── main.cpp
│   └── detectors/       # Detector implementations
├── test_samples/        # Vulnerable code examples
└── docs/                # Documentation

```

### Core Components

**Lexer** (`lexer.cpp`)
- Implements finite state machine for pattern recognition
- Handles comments, strings, and complex operators
- Provides token stream for parser

**Parser** (`parser.cpp`)
- Recursive descent parser for C/C++ subset
- Builds Abstract Syntax Tree (AST)
- Error recovery and reporting

**AST** (`ast.h`, `ast.cpp`)
- Complete node hierarchy for expressions, statements, declarations
- Visitor pattern for traversal
- Symbol table for scope management

**Detectors** (`src/detectors/`)
- Base class with common vulnerability reporting interface
- Each detector implements AST visitor pattern
- Configurable severity levels and remediation recommendations

## 🚀 Building and Usage

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or higher
- Git (for cloning)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/SafeC.git
cd SafeC

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j4

# Run SafeC
./safec ../test_samples/buffer_overflow_examples.c
```

### Usage Examples

**Basic Analysis:**
```bash
./safec vulnerable_code.c
```

**Generate JSON Report:**
```bash
./safec -o report.json vulnerable_code.c
```

**Help:**
```bash
./safec --help
```

### Example Output

```
================================================================================
SafeC Static Analysis Report
File: buffer_overflow_examples.c
================================================================================

Found 3 potential vulnerabilities:

[1] Buffer Overflow - HIGH
    Location: Line 9, Column 5
    Description: Use of unsafe function 'strcpy' which does not perform bounds checking
    Recommendation: Use strncpy() instead and ensure proper null termination

[2] Buffer Overflow - CRITICAL
    Location: Line 22, Column 5
    Description: Array 'arr' accessed with out-of-bounds index 10 (array size: 5)
    Recommendation: Ensure array indices are within bounds [0, 4]

--------------------------------------------------------------------------------
Summary:
  CRITICAL: 1
  HIGH:     2
================================================================================
```

## 📊 Research Context

### Motivation

Static analysis is a fundamental technique in cybersecurity for identifying vulnerabilities before code execution. This project demonstrates:

1. **Understanding of Program Analysis Theory**: Implementation of lexical analysis, parsing, and semantic analysis from first principles
2. **Practical Security Knowledge**: Focus on real-world vulnerability classes (CWE Top 25)
3. **Software Engineering Skills**: Clean architecture, modular design, comprehensive documentation

### Related Work

SafeC builds upon established static analysis techniques while providing an educational implementation:

- **Clang Static Analyzer**: Industrial-strength analyzer using LLVM infrastructure
- **Cppcheck**: Lightweight static analyzer with focus on low false positives
- **Coverity**: Commercial tool with advanced dataflow analysis
- **Infer**: Facebook's static analyzer using separation logic

### Novel Contributions

While SafeC is educational in scope, it demonstrates:
- Clean separation of concerns between lexing, parsing, and analysis
- Extensible detector architecture using visitor pattern
- Research-quality documentation and code organization
- Practical examples of common vulnerability patterns

### Limitations and Future Work

**Current Limitations:**
- Simplified C/C++ grammar (subset of full language)
- Basic dataflow analysis (no inter-procedural analysis)
- Limited path sensitivity in control flow analysis

**Future Enhancements:**
- Extend grammar support for full C/C++ compliance
- Implement inter-procedural analysis
- Add taint analysis for tracking user input
- Machine learning-based false positive reduction
- Integration with CI/CD pipelines

## 🎓 Academic Applications

This project is designed to demonstrate research capabilities for graduate school applications in cybersecurity. It showcases:

✅ **Technical Depth**: Understanding of compiler theory and program analysis  
✅ **Security Focus**: Knowledge of common vulnerability classes and detection techniques  
✅ **Research Methodology**: Systematic approach to problem-solving  
✅ **Documentation**: Clear communication of technical concepts  
✅ **Practical Skills**: Applicable to bug bounty hunting and security auditing

## 📚 References

1. Aho, A. V., Lam, M. S., Sethi, R., & Ullman, J. D. (2006). *Compilers: Principles, Techniques, and Tools* (2nd ed.)
2. Chess, B., & West, J. (2007). *Secure Programming with Static Analysis*
3. MITRE. (2023). *CWE Top 25 Most Dangerous Software Weaknesses*
4. Bessey, A., et al. (2010). "A few billion lines of code later: using static analysis to find bugs in the real world." *CACM*

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👤 Author

**Ronak Parmar**  
Software Engineer | Cybersecurity Researcher  
2.5 years experience in C/C++ and systems engineering

---

**Note**: This is a research and educational project. For production use, consider industrial-strength tools like Clang Static Analyzer, Coverity, or PVS-Studio.

