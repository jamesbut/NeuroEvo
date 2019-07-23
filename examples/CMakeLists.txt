include_directories(../include)

set(neuroEvo_LIBS ${CMAKE_SOURCE_DIR}/build/libneuroEvo.so)

add_executable(and_example
    and_example.cpp
)

add_executable(quadratic_function_example
    quadratic_function_example.cpp
)

add_executable(sequence_classification_example
    sequence_classification_example.cpp
)

add_executable(single_cart_pole_example
    single_cart_pole_example.cpp
)

target_link_libraries(and_example
    ${neuroEvo_LIBS}
)

target_link_libraries(quadratic_function_example
    ${neuroEvo_LIBS}
)

target_link_libraries(sequence_classification_example
    ${neuroEvo_LIBS}
)

target_link_libraries(single_cart_pole_example
    ${neuroEvo_LIBS}
)
