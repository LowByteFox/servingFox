cmake_minimum_required(VERSION 3.22)
project(servingfox VERSION 0.1 LANGUAGES C CXX)

# emit compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# C++ config
message("Configuring C++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")

# C config
message("Configuring C")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic")

# Include directories
message("Configuring include directories")
include_directories(utils/)

# libraries
message("Configuring libraries")
add_library(crashtrace STATIC utils/crashtrace.c)
add_library(logger STATIC utils/logger.c)

# getting files
FILE(GLOB_RECURSE SRC ./src/*.cpp ./src/*.hpp)

# executables
message("Configuring executables")
add_executable(servingfox ${SRC})
target_link_libraries(servingfox crashtrace logger)