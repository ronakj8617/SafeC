cmake_minimum_required(VERSION 3.15)
project(SafeC VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SAFEC_SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/utils.cpp
    src/report_generator.cpp
    src/config_loader.cpp
    src/detectors/buffer_overflow_detector.cpp
    src/detectors/use_after_free_detector.cpp
    src/detectors/memory_leak_detector.cpp
    src/detectors/format_string_detector.cpp
    src/detectors/integer_overflow_detector.cpp
    src/detectors/pointer_safety_detector.cpp
)

# Main executable
add_executable(safec src/main.cpp ${SAFEC_SOURCES})

# Optional: Enable testing
option(BUILD_TESTS "Build test suite" OFF)

if(BUILD_TESTS)
    enable_testing()
    
    # Download and configure Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
    
    # Test executable
    add_executable(safec_tests
        tests/test_lexer.cpp
        tests/test_parser.cpp
        tests/test_buffer_overflow.cpp
        tests/test_use_after_free.cpp
        tests/test_memory_leak.cpp
        tests/test_format_string.cpp
        tests/test_integer_overflow.cpp
        ${SAFEC_SOURCES}
    )
    
    target_link_libraries(safec_tests gtest gtest_main)
    
    add_test(NAME SafeCTests COMMAND safec_tests)
endif()

# Installation
install(TARGETS safec DESTINATION bin)
