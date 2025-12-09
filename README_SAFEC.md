# SafeC - Static C/C++ Security Analyzer

A comprehensive static analyzer for detecting security vulnerabilities in C/C++ code, with automated CI/CD pipeline for continuous analysis.

## Features

- **Buffer Overflow Detection**
- **Use-After-Free Detection**
- **Memory Leak Detection**
- **Format String Vulnerabilities**
- **Integer Overflow Detection**
- **Pointer Safety Issues**

## Quick Start

```bash
./build/safec -d --html report.html /path/to/code
```

## CI/CD Pipeline

Automated analysis runs on every push to `main` with reports published to GitHub Pages.

Visit: https://ronakj8617.github.io/SafeC/
