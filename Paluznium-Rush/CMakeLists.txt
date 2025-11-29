cmake_minimum_required(VERSION 3.10)

# set the project name
project(Genetic_Algorithm)

# Add openmp support
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
# Include FetchContent for downloading dependencies
include(FetchContent)

FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# set the include path
include_directories(include)

# add the main code
add_subdirectory(src)

# add the tests
include(CTest)
enable_testing()
add_subdirectory(tests)

# add the plotting
add_subdirectory(plotting)


