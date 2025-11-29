## add the genetic algorithm library
add_library(geneticAlgorithm Genetic_Algorithm.cpp Genetic_Algorithm_ALPS.cpp performance_utilities.cpp config.cpp)

set_target_properties( geneticAlgorithm
    PROPERTIES
    CXX_STANDARD 17
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# build the circuit simulator as a testable library
add_library(circuitSimulator CCircuit.cpp CSimulator.cpp simulate_flow.cpp post_process.cpp ../plotting/plot_circuit.cpp)
target_link_libraries(circuitSimulator PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(geneticAlgorithm PRIVATE nlohmann_json::nlohmann_json)
set_target_properties( circuitSimulator
    PROPERTIES
    CXX_STANDARD 17
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# build the executable
add_executable(Circuit_Optimizer main.cpp)
target_link_libraries(Circuit_Optimizer PUBLIC geneticAlgorithm circuitSimulator OpenMP::OpenMP_CXX)
target_link_libraries(Circuit_Optimizer PRIVATE nlohmann_json::nlohmann_json)

set_target_properties( Circuit_Optimizer
    PROPERTIES
    CXX_STANDARD 17
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
