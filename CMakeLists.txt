cmake_minimum_required (VERSION 3.8)

project ("VirtualFunctionPerf")
set (CMAKE_CXX_STANDARD 14)

add_executable(
  vtable_benchmark
  "main.cpp"
  "PerfTimer.cpp"
  "PerfTimer.h"
)

add_executable(
  greeter
  "Greeter.cpp"
)

add_executable(
  greeter_virtual
  "GreeterVirtual.cpp"
)
