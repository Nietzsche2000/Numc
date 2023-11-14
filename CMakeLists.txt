set(GCC_COVERAGE_COMPILE_FLAGS "-mavx -mfma")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(testingMonish testingMonish.c AssertionTests.c matrix.c printers.c)