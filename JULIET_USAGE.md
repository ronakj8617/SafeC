# Juliet Test Suite Integration Guide

This guide explains how to use the Juliet Test Suite with SafeC for comprehensive vulnerability detection testing.

## Overview

The Juliet Test Suite is a comprehensive collection of test cases for software security tools. It contains thousands of C/C++ test cases covering various Common Weakness Enumeration (CWE) categories.

## Prerequisites

1. **Build SafeC**: Ensure the SafeC executable is built
   ```bash
   mkdir -p build
   cd build
   cmake ..
   cmake --build .
   cd ..
   ```

2. **Juliet Test Suite**: The test suite should be located at `./juliet-test-suite-c/`

## Available Scripts

### 1. `juliet_test.py` - Quick Testing (Recommended for First Run)

Tests SafeC on a small subset of Juliet test cases (default: 10 files).

**Usage:**
```bash
python3 juliet_test.py
```

**Configuration:**
Edit the script to customize:
- `max_files`: Number of files to analyze (default: 10)
- `specific_cwe`: Test a specific CWE category (e.g., "CWE121_Stack_Based_Buffer_Overflow")

**Example - Test specific CWE:**
```python
# In juliet_test.py, modify:
max_files = 50
specific_cwe = "CWE121_Stack_Based_Buffer_Overflow"
```

**Output:**
- Console: Progress and summary statistics
- `juliet_test_results.json`: Detailed results for analyzed files

---

### 2. `juliet.py` - Full Suite Analysis

Analyzes **all** C files in the Juliet test suite (can take hours).

**Usage:**
```bash
python3 juliet.py
```

**Features:**
- Progress tracking with file counter
- 30-second timeout per file
- Error handling and reporting
- Comprehensive statistics

**Output:**
- Console: Real-time progress and final summary
- `juliet_results.json`: Complete results for all analyzed files

**Expected Runtime:**
- ~64,000+ C files in the full suite
- Estimated time: Several hours (depends on system performance)

---

### 3. `juliet_analyze.py` - Results Analysis

Generates a comprehensive summary report from the results JSON file.

**Usage:**
```bash
# Analyze full suite results
python3 juliet_analyze.py juliet_results.json

# Analyze test results
python3 juliet_analyze.py juliet_test_results.json
```

**Output:**
- Console: Summary statistics including:
  - Overall detection rates
  - Vulnerability types detected
  - CWE category breakdown
  - Detection percentage per category
- `*_findings.txt`: Detailed findings for all files with vulnerabilities

---

## Workflow Recommendations

### First-Time Setup

1. **Quick validation** (5-10 minutes):
   ```bash
   python3 juliet_test.py
   python3 juliet_analyze.py juliet_test_results.json
   ```

2. **Verify SafeC is working** by checking:
   - No build/path errors
   - Some vulnerabilities detected
   - Reasonable analysis time per file

3. **Test specific CWE categories** of interest:
   ```bash
   # Edit juliet_test.py to set specific_cwe
   python3 juliet_test.py
   ```

### Full Suite Analysis

1. **Run the full suite** (best done overnight):
   ```bash
   nohup python3 juliet.py > juliet_run.log 2>&1 &
   ```

2. **Monitor progress**:
   ```bash
   tail -f juliet_run.log
   ```

3. **Analyze results**:
   ```bash
   python3 juliet_analyze.py juliet_results.json
   ```

---

## Understanding Results

### JSON Output Format

Each analyzed file has the following structure:

```json
{
  "file": "/path/to/test/file.c",
  "stdout": "SafeC analysis output",
  "stderr": "Any error messages",
  "return_code": 0
}
```

Or for errors:
```json
{
  "file": "/path/to/test/file.c",
  "error": "Timeout after 30 seconds"
}
```

### Analysis Report Sections

1. **Overall Statistics**: Total files, detection rate, error rate
2. **Vulnerability Types**: Count of each vulnerability type detected
3. **CWE Category Breakdown**: Detection rates per CWE category

---

## Troubleshooting

### Issue: "SafeC executable not found"

**Solution:**
```bash
cd build
cmake --build .
cd ..
```

### Issue: "Test suite not found"

**Solution:** Ensure the Juliet test suite is in the correct location:
```bash
ls juliet-test-suite-c/testcases/
```

### Issue: Many timeouts

**Possible causes:**
- SafeC hanging on certain file patterns
- Timeout too short for complex files

**Solutions:**
1. Increase timeout in the script (change `timeout=30` to higher value)
2. Investigate specific files causing timeouts
3. Add more robust error handling in SafeC

### Issue: Low detection rate

**Investigation steps:**
1. Check specific CWE categories SafeC is designed to detect
2. Review sample findings to ensure output format is correct
3. Verify SafeC detectors are enabled in configuration

---

## Performance Tips

1. **Use SSD storage** for faster file I/O
2. **Run in background** for long analyses: `nohup python3 juliet.py &`
3. **Test incrementally** by CWE category before full suite
4. **Adjust timeout** based on your system performance

---

## CWE Categories in Juliet

The test suite includes (but is not limited to):

- **CWE-121**: Stack-based Buffer Overflow
- **CWE-122**: Heap-based Buffer Overflow  
- **CWE-134**: Uncontrolled Format String
- **CWE-190**: Integer Overflow
- **CWE-191**: Integer Underflow
- **CWE-415**: Double Free
- **CWE-416**: Use After Free
- **CWE-476**: NULL Pointer Dereference
- **CWE-787**: Out-of-bounds Write

And many more...

---

## Next Steps

After running the Juliet test suite:

1. **Review detection rates** per CWE category
2. **Investigate false negatives** (known vulnerabilities not detected)
3. **Analyze false positives** (incorrect detections)
4. **Improve detectors** based on findings
5. **Benchmark** against other static analysis tools

---

## Additional Resources

- [Juliet Test Suite Documentation](https://samate.nist.gov/SARD/test-suites/112)
- [CWE Database](https://cwe.mitre.org/)
- SafeC Documentation: `README.md`

---

**Last Updated:** December 2025
