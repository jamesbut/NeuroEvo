include_directories(../include)

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

add_executable(single_cart_pole_render_example
    single_cart_pole_render_example.cpp
)

add_executable(inverse_dct_example
    inverse_dct_example.cpp
)

target_link_libraries(and_example
    neuroEvo
)

target_link_libraries(quadratic_function_example
    neuroEvo
)

target_link_libraries(sequence_classification_example
    neuroEvo
)

target_link_libraries(single_cart_pole_example
    neuroEvo
)

target_link_libraries(single_cart_pole_render_example
    neuroEvo
)

target_link_libraries(inverse_dct_example
    neuroEvo
)