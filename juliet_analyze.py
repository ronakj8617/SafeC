#!/usr/bin/env python3
"""
Analyze the results from Juliet test suite runs and generate a summary report.
"""

import json
import sys
from collections import defaultdict

def analyze_results(results_file="juliet_results.json"):
    """Analyze the Juliet test results and generate a summary."""
    
    try:
        with open(results_file, 'r') as f:
            results = json.load(f)
    except FileNotFoundError:
        print(f"ERROR: Results file '{results_file}' not found.")
        print("Please run juliet.py first to generate results.")
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"ERROR: Invalid JSON in '{results_file}'")
        sys.exit(1)
    
    # Statistics
    total_files = len(results)
    files_with_errors = 0
    files_with_findings = 0
    files_clean = 0
    timeout_count = 0
    
    # CWE-based statistics
    cwe_stats = defaultdict(lambda: {"total": 0, "findings": 0, "errors": 0})
    
    # Vulnerability type statistics
    vuln_types = defaultdict(int)
    
    for result in results:
        file_path = result.get("file", "")
        
        # Extract CWE category from path
        cwe = "Unknown"
        if "CWE" in file_path:
            parts = file_path.split("/")
            for part in parts:
                if part.startswith("CWE"):
                    cwe = part
                    break
        
        cwe_stats[cwe]["total"] += 1
        
        # Check for errors
        if "error" in result:
            files_with_errors += 1
            cwe_stats[cwe]["errors"] += 1
            if "Timeout" in result["error"]:
                timeout_count += 1
            continue
        
        # Check for findings
        stdout = result.get("stdout", "")
        stderr = result.get("stderr", "")
        
        if stdout and stdout.strip():
            files_with_findings += 1
            cwe_stats[cwe]["findings"] += 1
            
            # Count vulnerability types
            if "Buffer Overflow" in stdout:
                vuln_types["Buffer Overflow"] += 1
            if "Use After Free" in stdout:
                vuln_types["Use After Free"] += 1
            if "Memory Leak" in stdout:
                vuln_types["Memory Leak"] += 1
            if "Format String" in stdout:
                vuln_types["Format String"] += 1
            if "Integer Overflow" in stdout:
                vuln_types["Integer Overflow"] += 1
            if "Null Pointer" in stdout:
                vuln_types["Null Pointer"] += 1
        else:
            files_clean += 1
    
    # Print summary report
    print("=" * 80)
    print("JULIET TEST SUITE ANALYSIS REPORT")
    print("=" * 80)
    print(f"\nOVERALL STATISTICS:")
    print(f"  Total files analyzed:     {total_files}")
    print(f"  Files with findings:      {files_with_findings} ({files_with_findings/total_files*100:.1f}%)")
    print(f"  Files clean:              {files_clean} ({files_clean/total_files*100:.1f}%)")
    print(f"  Files with errors:        {files_with_errors} ({files_with_errors/total_files*100:.1f}%)")
    print(f"  Timeouts:                 {timeout_count}")
    
    print(f"\nVULNERABILITY TYPES DETECTED:")
    if vuln_types:
        for vuln_type, count in sorted(vuln_types.items(), key=lambda x: x[1], reverse=True):
            print(f"  {vuln_type:25s} {count:5d}")
    else:
        print("  No vulnerabilities detected")
    
    print(f"\nCWE CATEGORY BREAKDOWN:")
    print(f"  {'CWE Category':<50s} {'Total':>8s} {'Findings':>10s} {'Errors':>8s} {'Detection %':>12s}")
    print(f"  {'-'*50} {'-'*8} {'-'*10} {'-'*8} {'-'*12}")
    
    for cwe, stats in sorted(cwe_stats.items()):
        total = stats["total"]
        findings = stats["findings"]
        errors = stats["errors"]
        detection_rate = (findings / total * 100) if total > 0 else 0
        print(f"  {cwe:<50s} {total:8d} {findings:10d} {errors:8d} {detection_rate:11.1f}%")
    
    print("\n" + "=" * 80)
    
    # Generate detailed findings file
    findings_file = results_file.replace(".json", "_findings.txt")
    with open(findings_file, 'w') as f:
        f.write("DETAILED FINDINGS REPORT\n")
        f.write("=" * 80 + "\n\n")
        
        for result in results:
            stdout = result.get("stdout", "")
            if stdout and stdout.strip():
                f.write(f"File: {result['file']}\n")
                f.write("-" * 80 + "\n")
                f.write(stdout)
                f.write("\n" + "=" * 80 + "\n\n")
    
    print(f"Detailed findings saved to: {findings_file}")
    print("=" * 80)

if __name__ == "__main__":
    results_file = sys.argv[1] if len(sys.argv) > 1 else "juliet_results.json"
    analyze_results(results_file)
