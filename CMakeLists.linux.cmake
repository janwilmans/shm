project(shm)

option(BUILD_WITH_ASAN "Build with address sanitizer" OFF)
option(BUILD_WITH_UBSAN "Build with undefined behaviour sanitizer" OFF)
option(BUILD_WITH_TSAN "Build with thread sanitizer" OFF)

enable_testing()
include(CTest)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message("No build type specified, defaulting to Release (full optimizations, no debug symbols), use -DCMAKE_BUILD_TYPE=RelWithDebInfo or =Debug for a debug configuration)")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

add_compile_options(-Wall -Wextra -pedantic -Wmissing-include-dirs -Wformat=2 -Wunused -Wno-error=unused-variable -Wcast-align -Wno-vla)

# this can only be applied to C++ sources
set(CMAKE_CXX_FLAGS "-Wnon-virtual-dtor -Woverloaded-virtual")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.3 AND NOT CLANG_ANALYZER)
    message("Adding GCC >= 8.3 compilation options")
    add_compile_options(-Wnull-dereference)
endif()

add_compile_options(-Werror)
add_compile_options(-fdiagnostics-color=auto)

if(BUILD_WITH_ASAN)
    add_compile_options(-fsanitize=address)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    message("-- build with ASAN enabled")
endif(BUILD_WITH_ASAN)

if(BUILD_WITH_UBSAN)
    add_compile_options(-fsanitize=undefined)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    message("-- build with UBSAN enabled")
endif(BUILD_WITH_UBSAN)

if(BUILD_WITH_TSAN)
    if(BUILD_WITH_ASAN OR BUILD_WITH_UBSAN)
      message(FATAL_ERROR "TSAN cannot be combined with ASAN or UBSAN")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.3)
      message(FATAL_ERROR "TSAN not working on older gcc versions (technically could work but needs recompilation of ALL dependencies using -fPIC and linking using -pie or -shared)")
    endif()

    add_compile_options(-fsanitize=thread -g -O1)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
    message("-- build with TSAN enabled")
endif(BUILD_WITH_TSAN)

add_subdirectory(sharedmemory)
add_subdirectory(gsl)

if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
    message(WARNING "32bit builds not supported / untested")
    set(TARGETBITS 32)
else()
    set(TARGETBITS 64)
endif()

message("")
message("CMAKE_MODULE_PATH: 		  ${CMAKE_MODULE_PATH}")
message("configured to run on:    ${CMAKE_SYSTEM_NAME} ${TARGETBITS} bit, ${CMAKE_BUILD_TYPE} Mode")
message("will be built in:        ${CMAKE_BINARY_DIR}")
message("CMAKE_C_COMPILER:        ${CMAKE_C_COMPILER}")
message("CMAKE_CXX_COMPILER:      ${CMAKE_CXX_COMPILER}")
message("will be installed in:    ${CMAKE_INSTALL_PREFIX}")
message("\n")
