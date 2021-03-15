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
if(${SFML_FOUND})
    set(SFML_FOUND 1)
else()
    set(SFML_FOUND 0)
endif()
#SFML is working right now
set(SFML_FOUND 0)

#Torch is on by default
if(NOT DEFINED WITH_TORCH)
    set(WITH_TORCH ON)
endif()
#Add torch path to cmake path
if(${WITH_TORCH})
    if(LINUX)
        set(CMAKE_PREFIX_PATH "/usr/local/lib/python3.8/dist-packages")
    else()
        #set(CMAKE_PREFIX_PATH "/usr/local/lib/python3.7/site-packages")
        #set(CMAKE_PREFIX_PATH "/Users/james/opt/anaconda3/envs/gym_api/lib/python3.7/site-packages")
        #set(CMAKE_PREFIX_PATH "/Users/james/Documents/projects/pytorch")
        set(CMAKE_PREFIX_PATH "/Users/james/libraries/pytorch")
    endif()
    find_package(Torch REQUIRED)
endif()
if(${Torch_FOUND})
    set(Torch_FOUND 1)
else()
    set(Torch_FOUND 0)
endif()
if(${WITH_TORCH} AND ${Torch_FOUND})
    set(USE_TORCH 1)
else()
    set(USE_TORCH 0)
endif()

# Set path of population data directory
set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(NEURO_EVO_CMAKE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Also set the variable in the parent scope so it
# can be used in other cmake files
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data" PARENT_SCOPE)
    set(NEURO_EVO_CMAKE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
    set(USE_TORCH ${USE_TORCH} PARENT_SCOPE)
    set(SFML_FOUND ${SFML_FOUND} PARENT_SCOPE)
endif()

# Make variable available to the C++ source code
add_compile_definitions(DATA_PATH="${DATA_PATH}")
add_compile_definitions(NEURO_EVO_CMAKE_SRC_DIR="${NEURO_EVO_CMAKE_SRC_DIR}")
add_compile_definitions(SFML_FOUND=${SFML_FOUND})
add_compile_definitions(USE_TORCH=${USE_TORCH})

# Build all files in src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Remove torch related files from compilation
if(NOT ${USE_TORCH})
    list(REMOVE_ITEM SOURCES
         "${NEURO_EVO_CMAKE_SRC_DIR}/src/phenotype/neural_network/torch_network.cpp"
         "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch_utils.cpp"
         "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/generative_models/autoencoder.cpp"
         "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/generative_models/gan.cpp"
         "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/generative_models/vae.cpp")
endif()

add_library(neuroEvo SHARED
    ${SOURCES}
)

target_link_libraries(neuroEvo
    ${Boost_LIBRARIES}
)

if(${USE_TORCH})
    target_link_libraries(neuroEvo
        ${TORCH_LIBRARIES}
    )
endif()

if(${SFML_FOUND})
    target_link_libraries(neuroEvo
        sfml-graphics
    )
endif()

target_include_directories(neuroEvo PUBLIC
    include
    ${Boost_INCLUDE_DIRS}
    #${TORCH_INCLUDE_DIRS}
    lib/eigen
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
