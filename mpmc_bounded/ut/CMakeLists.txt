find_package(Catch2 3 REQUIRED)

add_executable(mpmc_bounded_test test.cpp)

target_include_directories(mpmc_bounded_test PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(mpmc_bounded_test
    PRIVATE
        Catch2::Catch2WithMain
        mpmc_bounded
)

include(CTest)
include(Catch)
catch_discover_tests(mpmc_bounded_test)