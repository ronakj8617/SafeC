#!/usr/bin/env python3
"""
Filtered Juliet test runner that skips problematic files.
Focuses on test cases that are more likely to work with SafeC.
"""

import os
import subprocess
import json
import time
import sys

# Configuration
suite_path = "./juliet-test-suite-c/testcases"
max_files = 10000  # Set to None for unlimited, or a number to limit
specific_cwes = [
    "CWE121_Stack_Based_Buffer_Overflow",
    "CWE122_Heap_Based_Buffer_Overflow",
    "CWE134_Uncontrolled_Format_String",
    "CWE190_Integer_Overflow",
    "CWE191_Integer_Underflow",
]

# Filters to skip problematic files
skip_patterns = [
    "w32_",           # Windows-specific code
    "_w32",           # Windows-specific code
    "wchar_t",        # Wide character issues on non-Windows
    "windows.h",      # Windows headers
]

output_json = []

print(f"Filtered Juliet Test Suite Analysis")
print(f"Suite path: {suite_path}")
if specific_cwes:
    print(f"Targeting CWEs: {', '.join(specific_cwes)}")
if max_files:
    print(f"Max files: {max_files}")
print("=" * 60)

# Check if SafeC executable exists
if not os.path.exists("./build/safec"):
    print("ERROR: SafeC executable not found at ./build/safec")
    print("Please build the project first using: cmake --build build")
    sys.exit(1)

analyzed_count = 0
skipped_count = 0
error_count = 0
start_time = time.time()

# Determine which directories to scan
scan_paths = []
if specific_cwes:
    for cwe in specific_cwes:
        cwe_path = os.path.join(suite_path, cwe)
        if os.path.exists(cwe_path):
            scan_paths.append(cwe_path)
        else:
            print(f"Warning: CWE directory not found: {cwe_path}")
else:
    scan_paths = [suite_path]

print(f"Scanning {len(scan_paths)} CWE categor{'y' if len(scan_paths) == 1 else 'ies'}...\n")

for scan_path in scan_paths:
    for root, dirs, files in os.walk(scan_path):
        for file in files:
            if not file.endswith(".c"):
                continue
            
            # Check if we've hit the max file limit
            if max_files and analyzed_count >= max_files:
                break
            
            # Apply filters
            should_skip = False
            for pattern in skip_patterns:
                if pattern in file or pattern in root:
                    should_skip = True
                    skipped_count += 1
                    break
            
            if should_skip:
                continue
            
            analyzed_count += 1
            full_path = os.path.join(root, file)
            rel_path = os.path.relpath(full_path)
            print(f"[{analyzed_count}] Analyzing: {rel_path}")

            # run your analyzer
            try:
                result = subprocess.run(["./build/safec", full_path],
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE,
                                        text=True,
                                        timeout=30)  # 30 second timeout per file

                # Show brief status
                if result.stdout and "vulnerabilities found: 0" not in result.stdout:
                    # Count vulnerabilities
                    vuln_count = result.stdout.count("Location: Line")
                    print(f"  ✓ Found {vuln_count} potential issue(s)")
                else:
                    print(f"  ✓ Clean")

                output_json.append({
                    "file": full_path,
                    "stdout": result.stdout,
                    "stderr": result.stderr,
                    "return_code": result.returncode
                })
            except subprocess.TimeoutExpired:
                print(f"  ⚠️  TIMEOUT")
                error_count += 1
                output_json.append({
                    "file": full_path,
                    "error": "Timeout after 30 seconds"
                })
            except Exception as e:
                print(f"  ❌ ERROR: {str(e)}")
                error_count += 1
                output_json.append({
                    "file": full_path,
                    "error": str(e)
                })
        
        if max_files and analyzed_count >= max_files:
            break
    
    if max_files and analyzed_count >= max_files:
        break

elapsed_time = time.time() - start_time

print("\n" + "=" * 60)
print("Filtered Analysis Complete!")
print(f"Total files analyzed: {analyzed_count}")
print(f"Files skipped (filters): {skipped_count}")
print(f"Errors encountered: {error_count}")
print(f"Time elapsed: {elapsed_time:.2f} seconds")
if analyzed_count > 0:
    print(f"Average time per file: {elapsed_time/analyzed_count:.2f} seconds")
print(f"\nResults saved to: juliet_filtered_results.json")
print("=" * 60)

with open("juliet_filtered_results.json", "w") as f:
    json.dump(output_json, f, indent=2)

print("\nTo analyze results, run:")
print("  python3 juliet_analyze.py juliet_filtered_results.json")
