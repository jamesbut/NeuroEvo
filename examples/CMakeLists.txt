include_directories(../include)

add_executable(and_example
    and_example.cpp
)

set(neuroEvo_LIBS ${CMAKE_SOURCE_DIR}/build/libneuroEvo.so)

target_link_libraries(and_example
    "${neuroEvo_LIBS}"
)
