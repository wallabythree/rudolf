cmake_minimum_required(VERSION 3.10)

# set the project name
project(
	rudolf
    VERSION 0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

# set input and output directories
set(SRC_DIR src)
set(SOURCES
    ${SRC_DIR}/${PROJECT_NAME}.c
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set compiler warning level
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# disable MSVC compiler warnings for unsafe string functions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} -lcurl -lsqlite3)

