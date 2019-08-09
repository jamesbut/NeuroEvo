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

# Set path of population data directory
set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Also set the variable in the parent scope so it
# can be used in other cmake files
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data" PARENT_SCOPE)
endif()

# Make variable available to the C++ source code
add_compile_definitions(DATA_PATH="${DATA_PATH}")
add_compile_definitions(CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

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
