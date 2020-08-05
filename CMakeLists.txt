cmake_minimum_required(VERSION 3.11.4)

# Must use C++-17 because optionals are used
set(CMAKE_CXX_STANDARD 17)

# Check whether linux or mac is being used
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(neuro_evo)

# Boost is required
set(Boost_USE_MULTITHREADED TRUE)
find_package(Boost REQUIRED COMPONENTS filesystem)

# SFML used for rendering
find_package(SFML COMPONENTS graphics)

#Torch
#Add torch path to cmake path
set(CMAKE_PREFIX_PATH "/usr/local/lib/python3.7/site-packages")
find_package(Torch REQUIRED)

# Set path of population data directory
set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(NEURO_EVO_CMAKE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Also set the variable in the parent scope so it
# can be used in other cmake files
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data" PARENT_SCOPE)
    set(NEURO_EVO_CMAKE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
    if(${SFML_FOUND})
        set(SFML_FOUND 1 PARENT_SCOPE)
    else()
        set(SFML_FOUND 0 PARENT_SCOPE)
    endif()
endif()

# Make variable available to the C++ source code
add_compile_definitions(DATA_PATH="${DATA_PATH}")
add_compile_definitions(NEURO_EVO_CMAKE_SRC_DIR="${NEURO_EVO_CMAKE_SRC_DIR}")
if(${SFML_FOUND})
    add_compile_definitions(SFML_FOUND=1)
else()
    add_compile_definitions(SFML_FOUND=0)
endif()

# Build all files in src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(neuroEvo SHARED
    ${SOURCES}
)

target_link_libraries(neuroEvo
    ${TORCH_LIBRARIES}
    ${Boost_LIBRARIES}
)

if(${SFML_FOUND})
    target_link_libraries(neuroEvo
        sfml-graphics
    )
endif()

target_include_directories(neuroEvo PUBLIC
    include
    ${Boost_INCLUDE_DIRS}
    #${TORCH_INCLUDE_DIRS}
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
