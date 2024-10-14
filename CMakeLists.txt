cmake_minimum_required(VERSION 3.10)

# Set project name and version
project(cross_platform_linker VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(linker
    src/main.cpp
    src/parser.cpp
    src/symbol_table.cpp
    src/relocation.cpp
    src/linker.cpp
    src/platform_detector.cpp
)



# Include headers (though we have no header files yet)
target_include_directories(linker PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(linker PUBLIC include)
