#!/usr/bin/env python3
"""
Test script for running SafeC on a subset of Juliet test cases.
Useful for quick validation before running the full suite.
"""

import os
import subprocess
import json
import time
import sys

# Configuration
suite_path = "./juliet-test-suite-c/testcases"
max_files = 10  # Limit to first N files for testing
specific_cwe = None  # Set to a CWE folder name to test specific category, e.g., "CWE121_Stack_Based_Buffer_Overflow"

output_json = []

print(f"Testing SafeC with Juliet test suite")
print(f"Suite path: {suite_path}")
if specific_cwe:
    print(f"Specific CWE: {specific_cwe}")
    suite_path = os.path.join(suite_path, specific_cwe)
print(f"Max files to analyze: {max_files}")
print("=" * 60)

# Check if SafeC executable exists
if not os.path.exists("./build/safec"):
    print("ERROR: SafeC executable not found at ./build/safec")
    print("Please build the project first using: cmake --build build")
    sys.exit(1)

# Check if test suite exists
if not os.path.exists(suite_path):
    print(f"ERROR: Test suite not found at {suite_path}")
    sys.exit(1)

analyzed_count = 0
error_count = 0
start_time = time.time()

for root, dirs, files in os.walk(suite_path):
    for file in files:
        if file.endswith(".c"):
            if analyzed_count >= max_files:
                break
            
            analyzed_count += 1
            full_path = os.path.join(root, file)
            print(f"\n[{analyzed_count}/{max_files}] Analyzing: {os.path.relpath(full_path)}")

            # run your analyzer
            try:
                result = subprocess.run(["./build/safec", full_path],
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE,
                                        text=True,
                                        timeout=30)  # 30 second timeout per file

                # Show a preview of the output
                if result.stdout:
                    preview = result.stdout[:200]
                    print(f"  ✓ Output preview: {preview}...")
                else:
                    print(f"  ✓ No issues found")
                
                if result.stderr:
                    print(f"  ⚠️  Stderr: {result.stderr[:200]}")

                output_json.append({
                    "file": full_path,
                    "stdout": result.stdout,
                    "stderr": result.stderr,
                    "return_code": result.returncode
                })
            except subprocess.TimeoutExpired:
                print(f"  ⚠️  TIMEOUT: Analysis took too long")
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
    
    if analyzed_count >= max_files:
        break

elapsed_time = time.time() - start_time

print("\n" + "=" * 60)
print("Test Analysis Complete!")
print(f"Total files analyzed: {analyzed_count}")
print(f"Errors encountered: {error_count}")
print(f"Time elapsed: {elapsed_time:.2f} seconds")
if analyzed_count > 0:
    print(f"Average time per file: {elapsed_time/analyzed_count:.2f} seconds")
print(f"\nResults saved to: juliet_test_results.json")
print("=" * 60)

with open("juliet_test_results.json", "w") as f:
    json.dump(output_json, f, indent=2)

print("\nTo run the full suite, use: python3 juliet.py")
