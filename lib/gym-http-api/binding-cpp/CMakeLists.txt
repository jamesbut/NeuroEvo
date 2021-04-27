set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake" ${CMAKE_MODULE_PATH})

find_package(JsonCpp REQUIRED)
find_package(CURL REQUIRED)

add_executable(rand_agent random_agent.cpp gym_binding.cpp)

target_include_directories(rand_agent PUBLIC
    ${JsonCpp_INCLUDE_DIRS}
)

target_link_libraries(rand_agent PUBLIC
    ${JsonCpp_LIBRARY}
    ${CURL_LIBRARIES}
)

add_library(gym_binding SHARED
    gym_binding.cpp
)

target_include_directories(gym_binding PUBLIC
    ${JsonCpp_INCLUDE_DIRS}
)

target_link_libraries(gym_binding PUBLIC
    ${JsonCpp_LIBRARY}
    ${CURL_LIBRARIES}
)
