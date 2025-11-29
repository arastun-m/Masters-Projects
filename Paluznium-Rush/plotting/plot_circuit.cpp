#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "CSimulator.h"
#include "simulate_flow.h"

using json = nlohmann::json;
using namespace std;


// Baseline 6 unit: {0, 3, 1, 3, 2, 3, 5, 4, 7, 6, 3, 3, 8};
// 10 unit from basic algorithm: {0, 1, 8, 9, 11, 3, 0, 9, 2, 1, 5, 3, 6, 3, 7, 3, 12, 3, 4, 10, 1}
// Best performance 490: {2, 4, 7, 3, 5, 3, 1, 8, 4, 3, 9, 3, 6, 3, 0, 4, 12, 10, 3, 3, 11}
 
vector<int> circuit_vector = {0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10,11,6,12,7};
string save_name = "baseline_circuit_diagram";

int main(int argc, char * argv[])
{ 
    // int vector_size = circuit_vector.size();
    // Simulator_Parameters simulator_parameters = {0.01, 1000, false};

    // // convert vector to array
    // int circuit_vector_array[vector_size];
    // for (int i = 0; i < vector_size; i++) {
    //     circuit_vector_array[i] = circuit_vector[i];
    // }

    // // Run simulation
    // double result = circuit_performance(vector_size, circuit_vector_array, simulator_parameters);

    // // Write save name to performance json file
    // json performance_data;
    
    // ifstream input_file("./plotting/data/performance_data.json");
    // if (input_file.is_open()) {
    //     input_file >> performance_data;
    //     input_file.close();
    // }

    // performance_data["name"] = save_name;

    // ofstream output_file("./plotting/data/performance_data.json");
    // if (!output_file.is_open()) {
    //     cout << "Error: Failed to open performance file for writing" << endl;
    //     return 1;
    // }
    // output_file << performance_data.dump(4);
    // output_file.close();

    // // Plot circuit diagram
    // cout << "Plotting circuit diagram..." << endl;
    // system("python ./plotting/plot.py");
    return 0;
}