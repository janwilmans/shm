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
