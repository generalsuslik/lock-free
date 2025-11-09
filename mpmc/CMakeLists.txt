add_library(mpmc
        mpmc_bounded.cpp
)

enable_testing()
add_subdirectory(ut)
