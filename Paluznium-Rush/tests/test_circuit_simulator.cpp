/**
 * @file test_circuit_simulator.cpp
 * @brief Unit tests for the circuit simulator and performance evaluation functions.
 */
#include <cmath>
#include <iostream>

#include "CSimulator.h"

// function to compare two floating-point values w tolerance
// Returns true if reult is within +-tolerance of 'expected'
/**
 * @brief Compare two floating-point values with a tolerance.
 * @param result The computed result.
 * @param expected The expected value.
 * @param tolerance Allowed difference between result and expected.
 * @return true if result is within ±tolerance of expected, false otherwise.
 */
bool expect_near(double result, double expected, double tolerance = 1.0) {
    return std::fabs(result - expected) < tolerance;
}

// function to run a test and print the result - one that we expect
/**
 * @brief Run a test and print the result, expecting a value near the expected result.
 * @param label Description of the test.
 * @param vec Pointer to the circuit vector.
 * @param size Size of the circuit vector.
 * @param expected Expected performance value.
 * @param tolerance Allowed difference between result and expected.
 */
void run_test(const std::string& label, int* vec, int size, double expected, double tolerance = 1.0) {
    std::cout << "\n[" << label << "]\n";
    double result = circuit_performance(size, vec);
    std::cout << "Result = " << result << " £/s\n";
    if (expect_near(result, expected, tolerance)) {
        std::cout << "PASS \n";
    } else {
        std::cout << "FAIL (Expected ≈ " << expected << ")\n";
    }
}

// funcgtion to run a test and print the result - one that is expected to be low
/**
 * @brief Run a test and print the result, expecting a low performance value.
 * @param label Description of the test.
 * @param vec Pointer to the circuit vector.
 * @param size Size of the circuit vector.
 * @param threshold Performance threshold for a "low" result.
 */
void run_low_test(const std::string& label, int* vec, int size, double threshold = 0.0) {
    std::cout << "\n[" << label << "]\n";
    double result = circuit_performance(size, vec);
    std::cout << "Result = " << result << " £/s\n";
    if (result < threshold) {
        std::cout << "PASS (Correctly low performance)\n";
    } else {
        std::cout << "FAIL (Expected < " << threshold << ")\n";
    }
}

// main function to run the tests above
/**
 * @brief Main function to run all circuit simulator tests.
 * @return Exit code.
 */
int main() {
    // Reference circuit (from assignment figure, known profit)
    int vec_ref[] = {
        0, 3, 1, 3, 2, 3, 5, 4, 7, 6, 3, 3, 8
    };
    run_test("Reference circuit (expected ≈ £301.91)", vec_ref, 13, 301.91, 2.0);

    // Tailings-only circuit
    int vec_tail[] = {
        0, 8, 8, 8, 8, 8, 8
    };
    run_low_test("Tailings-only circuit (expected low)", vec_tail, 7);

    // Waste in product streams
    int vec_bad_route[] = {
        0, 6, 7, 6, 7, 6, 7
    };
    run_low_test("Waste routed to products (expected negative)", vec_bad_route, 7);

    // Self-loop (unit feeds itself)
    int vec_loop[] = {
        0, 0, 1, 1, 2, 2, 3
    };
    run_low_test("Self-loop (invalid circuit)", vec_loop, 7);

    // Deep recirculation structure
    int vec_recycle[] = {
        0, 1, 2, 2, 3, 3, 4, 4, 0
    };
    run_low_test("Recirculation circuit (check convergence)", vec_recycle, 9);

    // more to add, eg. mixed ouputs to streams etc
}
