# build the plotting executable
add_executable(plot_circuit plot_circuit.cpp)

# link with the circuit simulator library
target_link_libraries(plot_circuit PUBLIC circuitSimulator)

# link with the json library
target_link_libraries(plot_circuit PUBLIC nlohmann_json::nlohmann_json)

set_target_properties(plot_circuit
    PROPERTIES
    CXX_STANDARD 17
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)