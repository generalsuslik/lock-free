find_package(Catch2 3 REQUIRED)

add_executable(spsc_bounded_test test.cpp)

target_include_directories(spsc_bounded_test PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(spsc_bounded_test
    PRIVATE
        Catch2::Catch2WithMain
        spsc_bounded
)

include(CTest)
include(Catch)
catch_discover_tests(spsc_bounded_test)