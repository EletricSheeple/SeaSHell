cmake_minimum_required(VERSION 3.10)

# Set the project name
project(SeaSHell VERSION 1.0.0 LANGUAGES C)

# Create a directory for binaries
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/corelib)

# Specify the source files for the library
set(LIBRARY_SOURCES
    src/corelib/term.c
    src/corelib/expander.c
    src/corelib/proc.c
    src/corelib/handle.c
    src/corelib/hist.c
    src/corelib/log.c
)

# Create a library from the specified sources
add_library(core
  STATIC ${LIBRARY_SOURCES}
)

# Specify the source file for the executable
set(EXECUTABLE_SOURCE
    src/main.c
)

# Create the executable and link it with the library
add_executable(SeaSHell ${EXECUTABLE_SOURCE})
target_link_libraries(SeaSHell PRIVATE core)

# Set output directories for the library
set_target_properties(core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
