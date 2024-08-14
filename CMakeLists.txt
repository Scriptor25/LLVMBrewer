cmake_minimum_required(VERSION 3.28)
project(LLVMBrewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Libraries: ${LLVM_LIBRARY_DIRS}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

file(GLOB_RECURSE libbrewer-src lib/src/*.cpp lib/include/*.hpp)
add_library(libbrewer STATIC ${libbrewer-src})
target_include_directories(libbrewer PUBLIC lib/include)
target_link_libraries(libbrewer PUBLIC ${LLVM_LIBRARY_DIRS}/LLVM-C.lib)

file(GLOB_RECURSE test-src test/src/*.cpp test/include/*.hpp)
add_executable(test ${test-src})
target_include_directories(test PRIVATE test/include)
target_link_libraries(test PRIVATE libbrewer)
