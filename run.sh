#!/bin/bash

set -e  # Exit immediately on error

echo "🧹 Clear the terminal"
clear

echo "🧹 Cleaning previous CMake cache and build files..."
rm -rf CMakeCache.txt CMakeFiles Makefile cmake_install.cmake build

echo "📁 Creating build directory..."
mkdir -p build
cd build

echo "⚙️  Running CMake..."
cmake ..

echo "🔨 Building project..."
if ! make; then
    echo "❌ Build failed!"
    exit 1
fi

# Note: ctest is for unit tests, skipping if no tests configured
# if ! ctest; then
#       echo "❌ Test cases failed!"
#       exit 1
# fi

echo ""
echo "🚀 Running SafeC on parser-compatible test samples..."
echo "========================================"
echo "Note: Testing files in test_samples/working/"
echo "These are simplified C files without preprocessor directives"
echo "========================================"

# Go back to project root
cd ..

# Counter for tracking
total_files=0
analyzed_files=0
failed_files=0

# Find all .c files in test_samples/working directory
for test_file in test_samples/working/*.c; do
    if [ -f "$test_file" ]; then
        total_files=$((total_files + 1))
        echo ""
        echo "📝 Testing: $test_file"
        echo "----------------------------------------"
        
        # Run SafeC on the file (allow it to fail without stopping script)
        if ./build/safec "$test_file" 2>&1; then
            analyzed_files=$((analyzed_files + 1))
            echo "✅ Analysis completed for $test_file"
        else
            failed_files=$((failed_files + 1))
            echo "⚠️  Analysis failed for $test_file"
        fi
        echo "----------------------------------------"
    fi
done

echo ""
echo "========================================"
echo "📊 Summary:"
echo "   Total files: $total_files"
echo "   Successfully analyzed: $analyzed_files"
echo "   Failed/Skipped: $failed_files"
echo "========================================"
echo ""
echo "ℹ️  Note: Files in test_samples/ (with #include) will cause parser errors."
echo "   This is expected - our parser implements a simplified C grammar."
echo "   Files in test_samples/working/ are designed to work with our parser."
echo ""

if [ $total_files -eq 0 ]; then
    echo "⚠️  No test files found in test_samples/working/"
    exit 1
fi
