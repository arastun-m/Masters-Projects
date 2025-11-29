// /**
//  * @file test_genetic_init_select.cpp
//  * @brief This file contains the tests for initialisation and selection processes of the genetic algorithm.
//  */

// #include <iostream>
// #include <Genetic_Algorithm.h>


// /* ---------------- POPULATION INITIALISATION TESTS ---------------- */
// // int test_initialisation() {
// //     GeneticAlgorithm ga;
// //     if (ga.get_old_pop().size() != 0) { // should be empty at start
// //         return -1;
// //     }
// //     ga.init_population(); // size should be DEFAULT_ALGORITHM_PARAMETERS.pop_size
// //     if (ga.get_old_pop().size() != params_.pop_size) {
// //         return -1;
// //     }

// //     for (auto &ind : ga.get_old_pop()) { // check individual size and starting fitness value (0.0)
// //         if (ind.vector.size() != DEFAULT_ALGORITHM_PARAMETERS.circuit_size) {
// //             return -1;
// //         }
// //         if (ind.fitness != 0.0) {
// //             return -1;
// //         }
// //     }
// //     return 0;
// // }

// /* ---------------- SELECTION TESTS ---------------- */
// int test_selection() {
//     GeneticAlgorithm ga;
//     ga.init_population();

//     // test tournament selection
//     std::pair<Individual, Individual> parents = ga.select_tournament();
//     if (parents.first.vector.size() != DEFAULT_ALGORITHM_PARAMETERS.circuit_size ||
//         parents.second.vector.size() != DEFAULT_ALGORITHM_PARAMETERS.circuit_size) {
//         return -1;
//     }
//     // TODO: add more tests for selection methods

//     return 0;
// }


int main() {
    // test population initialisation
    // int return_code = test_initialisation();
    // if (return_code != 0) {
    //     std::cout << "Population initialisation failed!" << std::endl;
    //     return return_code;
    // }

    // // test selection
    // return_code = test_selection();
    // if (return_code != 0) {
    //     std::cout << "Selection process failed!" << std::endl;
    //     return return_code;
    // }

    // std::cout << "All tests passed!" << std::endl;
    // return return_code;
    return 0;
}
