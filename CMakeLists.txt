project(main)
cmake_minimum_required(VERSION 3.17)

if (MSVC)
    include(CMakeLists.msvc.cmake)
else()
    include(CMakeLists.linux.cmake)
endif()