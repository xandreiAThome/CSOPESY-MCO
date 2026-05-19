cmake_minimum_required(VERSION 3.16)
project(csopesy LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(csopesy
  src/console.cpp
  src/console_state.cpp
  src/main.cpp
  src/parser.cpp
  src/terminal_utils.cpp
)

target_compile_features(csopesy PRIVATE cxx_std_20)
target_include_directories(csopesy PRIVATE include)

target_compile_options(csopesy PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)
