add_library(mpmc_bounded
        mpmc_bounded.cpp
)

enable_testing()
add_subdirectory(ut)
