cmake_minimum_required(VERSION 3.21)
project(csopesy LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(csopesy
  src/console.cpp
  src/console_state.cpp
  src/main.cpp
  src/parser.cpp
  src/terminal_utils.cpp
 "include/scheduler/process.hpp" "include/scheduler/process.cpp")

target_compile_features(csopesy PRIVATE cxx_std_20)
target_include_directories(csopesy PRIVATE include)

add_custom_command(TARGET csopesy POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
		  "${PROJECT_SOURCE_DIR}/assets"
		  "$<TARGET_FILE_DIR:csopesy>/assets"
)

target_compile_options(csopesy PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)
