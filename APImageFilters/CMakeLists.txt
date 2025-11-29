cmake_minimum_required(VERSION 3.21)

# Conditional compiler setting
if(APPLE)
    # Looking for gcc installed by Homebrew
    find_program(GCC NAMES gcc-14 gcc-13 gcc-12 gcc-11)
    find_program(GXX NAMES g++-14 g++-13 g++-12 g++-11)
    if(GCC AND GXX)
        set(CMAKE_C_COMPILER ${GCC})
        set(CMAKE_CXX_COMPILER ${GXX})
    else()
        message(FATAL_ERROR "No suitable gcc/g++ compiler found. Please install gcc and g++.")
    endif()
elseif(UNIX)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Project name and version
project(APImageFilters VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add the executable
add_executable(APImageFilters
    src/main.cpp
    src/Image.cpp
    src/Filter.cpp
    src/Volume.cpp
    src/Slice.cpp
    src/Projection.cpp
)

# Enable testing
include(CTest)
enable_testing()

# Include the tests (leave this for now; we'll set up tests later)
include(${CMAKE_SOURCE_DIR}/cmdtests.cmake)

# Add a separate executable for RunAllTests (but do NOT add it to ctest)
add_executable(RunAllTests
    test/test_main.cpp
    test/test_histogram_equalization.cpp
    test/test_projection.cpp
    test/test_slice.cpp
    test/test_volume.cpp
    test/test_image.cpp
    test/test_3dfilters.cpp
    test/test_greyscale.cpp
    test/test_threshold.cpp
    test/test_brightness.cpp
    test/test_saltpepper.cpp
    test/test_boxblur.cpp
    test/test_medianblur.cpp
    test/test_gaussianblur.cpp
    test/test_sharpening.cpp
    test/test_edge_detection.cpp
    src/Filter.cpp
    src/Image.cpp
    src/Volume.cpp
    src/stb_image_impl.cpp
    src/Projection.cpp
    src/Slice.cpp
)

# Ensure the test image output directory exists
add_custom_target(CreateTestImagesDir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test/images
    COMMENT "Creating directory: ${CMAKE_BINARY_DIR}/test/images"
)