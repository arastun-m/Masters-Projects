/**
 * @file test_validity_checker.cpp
 * @brief Unit tests for the circuit validity checker functions in the Circuit class.
 */
#include <iostream>
#include <chrono>
#include "CUnit.h"
#include "CCircuit.h"

/**
 * @struct TestCaseD
 * @brief Test case structure for discrete circuit vector validity tests.
 */
struct TestCaseD {
    std::vector<int> vec;
    bool expect_valid;
};

/**
 * @struct TestCaseC
 * @brief Test case structure for circuit vector and parameter validity tests.
 */
struct TestCaseC {
    std::vector<int>   nodes;
    std::vector<double> weights;
    bool               expect;
};

/**
 * @brief Main function to run all circuit validity checker tests.
 * @return Exit code (0 if all tests pass, 1 otherwise).
 */
int main() {
    std::vector<TestCaseD> tests_d =
    {
        // assert is false
        {{0,1,2,2,3,4,5,6},    false},
        {{0,1,2,3},            false},
        {{4,1,2,3,0,4,5},      false},
        {{6,1,2,3,0,4,5},      false},
        {{1,2,3,0,3,4,3,7,6},  false},
        {{1,2,3,0,1,4,3,0,6},  false},
        {{0,1,4,3,0,4,5},      false},
        {{1,1,3,0,3,4,3,0,6},  false},
        {{0,1,1,3,2,4,5},      false},
        {{1,2,3,0,3,4,3,0,2},  false},
        {{0,1,2,4,0,5,5},      false},
        {{0,2,1,1,2,1,3,1,2},  false},
        {{0,3,1,3,2,3,5,1,7,0,3,3,8},                false},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,10,7,6,10},      false},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10,10,6,6,7},        false},
        {{0,3,4,3,2,3,5,4,9,8,3,3,6,5,7,6,10,11,6,12,7},       false},
        {{0,3,1,3,2,3,5,4,9,8,3,8,2,5,7,1,10,11,6,-11,7},       false},
        {{0,3,1,3,2,3,5,4,7,0,3,3,4,5,3,5,6,7,8,2,7,8,5,12,14},      false},

        // assert is true
        {{0,1,2,3,0,4,5},      true},
        {{1,2,3,0,3,4,3,0,6},  true},
        {{0,1,2,4,0,4,5},      true},
        {{0,1,2,4,0,4,5},      true},
        {{0,1,3,2,0,4,3,0,5},  true},
        {{0,1,2,3,0,0,4},      true},
        {{0,3,1,3,2,3,5,4,7,6,3,3,8},      true},
        {{0,3,1,3,2,3,5,4,7,0,3,3,8},      true},
        {{0,3,1,3,2,3,5,4,7,6,3,3,0},      true},
        {{0,3,1,3,2,3,5,4,7,5,3,3,8},      true},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10},       true},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10,11,6,12,7},       true},
        {{0,3,1,3,2,3,5,4,9,8,3,7,1,9,7,6,10,11,6,12,7},       true},
    };

    std::vector<TestCaseC> tests_c =
    {
        // another version
        {{0,1,2,3,0,4,5},{1.1,0.2,0.22},             false},
        {{1,2,3,0,3,4,3,0,6},{0.99,0.2,0.22},        false},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10},{0.099011,0.2,0.22,0.1,0.99,0.2,0.22},      false},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10},{0.1,1.01,0.2,0.22,0.1,0.99,0.2,0.22},      false},
        {{0,3,1,3,2,3,5,4,9,8,3,7,1,9,7,6,10,11,6,12,7},{0.1,0.99,0.2,0.22,0.1,0.99,0.2,0.22,1.01,0.99},       false},


        {{0,1,2,3,0,4,5},{0.1,0.2,0.22},             true},
        {{1,2,3,0,3,4,3,0,6},{0.1,0.99,0.2,0.22},    true},
        {{0,1,2,3,0,0,4},{0.34,0.22,0.0000000010},      true},
        {{0,3,1,3,2,3,5,4,7,5,3,3,8},{0.1,0.2,0.30,0.4000000,0.5,0.6040},      true},
        {{0,3,1,3,2,3,5,4,9,8,3,3,6,5,7,6,10},{0.1,0.99,0.2,0.22,0.1,0.99,0.2,0.22},      true},
        {{0,3,1,3,2,3,5,4,9,8,3,8,2,5,7,1,10,11,6,12,7},{0.1,0.99,0.2,0.22,0.1,0.99,0.2,0.98,0.97,0.22},       true},
    };

    // start time
    auto t_start = std::chrono::high_resolution_clock::now();

    int fail_count = 0;
    for (size_t i = 0; i < tests_d.size(); ++i) {
        auto& tc = tests_d[i];
        bool ok = Circuit::check_validity((int)tc.vec.size(), tc.vec.data());
        std::cout << "TestD " << i
            << " (size=" << tc.vec.size() << "): "
            << (ok ? "VALID" : "INVALID")
            << "   expected: " << (tc.expect_valid ? "VALID" : "INVALID");
        if (ok == tc.expect_valid) {
            std::cout << "   PASS\n";
        }
        else {
            std::cout << "   FAIL\n";
            ++fail_count;
        }
    }

    std::cout << std::endl;

    // test_c
    for (size_t i = 0; i < tests_c.size(); ++i) {
        const auto& tc = tests_c[i];
        bool ok = Circuit::check_validity(tc.nodes, tc.weights);
        std::cout << "TestC " << i
            << " (nodes=" << tc.nodes.size()
            << ", weights=" << tc.weights.size() << "): "
            << (ok ? "VALID" : "INVALID")
            << "   expected: "
            << (tc.expect ? "VALID" : "INVALID");
        if (ok == tc.expect) {
            std::cout << "   PASS\n";
        }
        else {
            std::cout << "   FAIL\n";
            ++fail_count;
        }
    }


    // end time
    auto t_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
    std::cout << "\nTotal test time: " << duration << " ms\n";

    if (fail_count == 0) {
        std::cout << "\nAll tests passed!\n";
        return 0;
    }
    else {
        std::cout << "\n" << fail_count << " test(s) failed.\n";
        return 1;
    }
}