import os
import subprocess
import json
import time

suite_path = "./juliet-test-suite-c/testcases"
output_json = []

print(f"Scanning Juliet test suite at: {suite_path}")
print("=" * 60)

# Check if SafeC executable exists
if not os.path.exists("./build/safec"):
    print("ERROR: SafeC executable not found at ./build/safec")
    print("Please build the project first using: cmake --build build")
    exit(1)

# Count total files first
total_files = sum(1 for root, dirs, files in os.walk(suite_path) 
                  for file in files if file.endswith(".c"))
print(f"Found {total_files} C files to analyze\n")

analyzed_count = 0
error_count = 0
start_time = time.time()

for root, dirs, files in os.walk(suite_path):
    for file in files:
        if file.endswith(".c"):
            analyzed_count += 1
            full_path = os.path.join(root, file)
            print(f"[{analyzed_count}/{total_files}] Analyzing: {full_path}")

            # run your analyzer
            try:
                result = subprocess.run(["./build/safec", full_path],
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE,
                                        text=True,
                                        timeout=30)  # 30 second timeout per file

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

elapsed_time = time.time() - start_time

print("\n" + "=" * 60)
print("Analysis Complete!")
print(f"Total files analyzed: {analyzed_count}")
print(f"Errors encountered: {error_count}")
print(f"Time elapsed: {elapsed_time:.2f} seconds")
print(f"Average time per file: {elapsed_time/analyzed_count:.2f} seconds")
print(f"\nResults saved to: juliet_results.json")
print("=" * 60)

with open("juliet_results.json", "w") as f:
    json.dump(output_json, f, indent=2)

