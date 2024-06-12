cmake_minimum_required(VERSION 3.0)
project(main)

set(CMAKE_CXX_STANDARD 14)

# Include the headers in the src directory
include_directories(${CMAKE_SOURCE_DIR}/src)

# Include the SystemC headers
include_directories(${CMAKE_SOURCE_DIR}/lib/systemc/include)

# Link the SystemC library
link_directories(${CMAKE_SOURCE_DIR}/lib/systemc/lib) # adjust this path as needed

set(SystemCLanguage_DIR "${CMAKE_SOURCE_DIR}/lib/systemc/lib/cmake/SystemCLanguage")
find_package(SystemCLanguage CONFIG REQUIRED)

# Add all the .c and .cpp files in the src directory to the executable
add_executable(main
        src/impl/simulation.h
        src/impl/simulation.cpp
        src/impl/tracefix.hpp
        src/main.c
        src/help.c
        src/types.h
        src/csv_parser.c)

target_link_libraries(main SystemC::systemc)