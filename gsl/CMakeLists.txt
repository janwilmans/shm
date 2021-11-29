project(gsl)

add_library(gsl_cmake_interface INTERFACE)
add_library(gsl::gsl ALIAS gsl_cmake_interface)

target_include_directories(gsl_cmake_interface INTERFACE include)
