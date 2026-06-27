cmake_minimum_required(VERSION 3.21)
project(csopesy LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(csopesy
  src/main.cpp
  src/globals.cpp
  src/console.cpp
  src/console_state.cpp
  src/parser.cpp
  src/terminal_utils.cpp
  src/scheduler/fcfsscheduler.cpp
  src/scheduler/process.cpp 
  src/scheduler/core_worker.cpp
  src/scheduler/ietthread.cpp
  src/scheduler/global_scheduler.cpp
  src/scheduler/process_factory.cpp
  src/commands/print_command.cpp
  src/commands/declare_command.cpp
  src/commands/operand.cpp
  src/commands/add_command.cpp
  src/commands/subtract_command.cpp
  src/commands/sleep_command.cpp
 "src/commands/for_command.cpp")

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
