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
#find_package(SFML COMPONENTS graphics)
#if(${SFML_FOUND})
#    set(SFML_FOUND 1)
#else()
#    set(SFML_FOUND 0)
#endif()
# SFML is not working right now
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
        set(CMAKE_PREFIX_PATH "~/libraries/libtorch")
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

# Find Python libs for python/C++ binding
find_package(PythonLibs REQUIRED)

# JSON library
find_package(nlohmann_json REQUIRED)

# Pretty table library
set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(lib/libfort)


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

# Build all files in src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Remove torch related files from compilation
if(NOT ${USE_TORCH})
    list(REMOVE_ITEM SOURCES
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/phenotype/neural_network/torch_network.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/batch_utils.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/module_utils.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/tensor_utils.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/weight_initialisation.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/schedulers/learning_rate_scheduler.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/schedulers/per_epoch_lr.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/schedulers/reduce_lr_on_plateau.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/torch/schedulers/step_lr.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/supervised_model.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/trainable_model.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/unsupervised_models/autoencoder.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/unsupervised_models/gan.cpp"
        "${NEURO_EVO_CMAKE_SRC_DIR}/src/util/models/unsupervised_models/vae.cpp")
endif()

add_library(neuroEvo SHARED
    ${SOURCES}
)

# Make variable available to the C++ source code
target_compile_definitions(neuroEvo PUBLIC
    -DDATA_PATH="${DATA_PATH}"
    -DNEURO_EVO_CMAKE_SRC_DIR="${NEURO_EVO_CMAKE_SRC_DIR}"
    -DSFML_FOUND=${SFML_FOUND}
    -DUSE_TORCH=${USE_TORCH}
)


target_link_libraries(neuroEvo
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    nlohmann_json::nlohmann_json
    fort
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
    lib/eigen
    ${PYTHON_INCLUDE_DIRS}
    lib/libfort/lib
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
