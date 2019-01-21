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

add_executable(
  greeter_manual_vtable
  "GreeterManualVTable.cpp"
)

add_executable(
  greeter_direct_vtable
  "GreeterDirectVTable.cpp"
)

add_executable(
  grouped_calls
  "GroupedCalls.cpp"
)

add_executable(
  override_check
  "OverrideCheck.cpp"
)
