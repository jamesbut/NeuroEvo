cmake_minimum_required(VERSION 3.11.4)

# Must use C++-17 because optionals are used
set(CMAKE_CXX_STANDARD 17)

# Check whether linux or mac is being used
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(neuro_evo)

# Boost is required
find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
)

# Build all files in src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(neuroEvo SHARED
    ${SOURCES}
)

target_link_libraries(neuroEvo
    ${Boost_LIBRARIES}
)

# Library used for parallelisation
if(LINUX)
    target_link_libraries(neuroEvo
        rt
    )
endif()

# Optional build examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(BUILD_EXAMPLES)
