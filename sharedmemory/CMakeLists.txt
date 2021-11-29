project(sharedmemory)

add_library(sharedmemory
    src/mmap.cc
    src/shmopen.cc
    src/errnoexception.cc
)

target_include_directories(sharedmemory PUBLIC include)

target_link_libraries(sharedmemory
    PRIVATE
        fmt::fmt
        pthread
        rt
    PUBLIC
        gsl::gsl
)

add_executable(sharedmemory_test
    tests/sharedmemory_test.cc
)

target_link_libraries(sharedmemory_test
    PRIVATE
        sharedmemory
        GTest::GTest
        GTest::Main        
)
