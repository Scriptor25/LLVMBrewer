cmake_minimum_required(VERSION 3.28)
project(LLVMBrewer)

option(BREWER_BUILD_EXAMPLE "Enable the example target" OFF)
option(BREWER_INSTALL "Enable the install targets" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})

file(GLOB_RECURSE brewer-src lib/src/*.cpp lib/include/*.hpp)
add_library(brewer STATIC ${brewer-src})
target_compile_definitions(brewer PUBLIC ${LLVM_DEFINITIONS_LIST})
target_include_directories(brewer PUBLIC lib/include ${LLVM_INCLUDE_DIRS})
target_link_libraries(brewer PUBLIC ${LLVM_AVAILABLE_LIBS})

if (${BREWER_INSTALL})
    install(TARGETS brewer)
endif ()

if (${BREWER_BUILD_EXAMPLE})
    file(GLOB_RECURSE example-src example/src/*.cpp example/include/*.hpp)
    add_executable(example ${example-src})
    target_include_directories(example PRIVATE example/include)
    target_link_libraries(example PRIVATE brewer)

    if (${BREWER_INSTALL})
        install(TARGETS example)
    endif ()
endif ()
