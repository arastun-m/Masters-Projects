/**
 * @file simulate_flow.h
 * @brief Declarations for structures and functions used to simulate material flow in a mineral processing circuit.
 */
#pragma once
#include "CUnit.h"
#include <vector>
#include <cmath>
#include <iostream>
#include "CSimulator.h"

/**
 * @struct Stream
 * @brief Represents a material stream with palusznium, gormanium, and waste components.
 */
struct Stream {
    double pal   = 0.0;     ///< Palusznium content
    double gor   = 0.0;     ///< Gormanium content
    double waste = 0.0;     ///< Waste content
    
    /**
     * @brief Reset all stream components to zero.
     */
    void clear() { pal = gor = waste = 0.0; }

    /**
     * @brief Compute the sum of absolute differences between this stream and another.
     * @param other The other stream to compare.
     * @return The sum of absolute differences for all components.
     */    
    double diff(const Stream& other) const {
        return std::fabs(pal   - other.pal)   +
               std::fabs(gor   - other.gor)   +
               std::fabs(waste - other.waste);
    }

    /**
     * @brief Add another stream's components to this stream.
     * @param rhs The stream to add.
     * @return Reference to this stream after addition.
     */
    Stream& operator+=(const Stream& rhs) {
        pal   += rhs.pal;
        gor   += rhs.gor;
        waste += rhs.waste;
        return *this;
    }
};

// Unit structure defined by the circuit layout
/**
 * @struct Unit
 * @brief Represents a processing unit in the circuit, including routing and kinetic model.
 */
struct Unit {
    int conc_dest = -1;         ///< Destination index for concentrate stream
    int tail_dest = -1;         ///< Destination index for tailings stream

    Stream feed;                ///< Current input feed
    Stream conc;                ///< Output concentrate stream
    Stream tail;                ///< Output tailings stream

    CUnit  cell;                ///< Kinetic model for the unit
    // Constructor to directly initialize routing and kinetics model
    // Used when converting from parsed circuit structure
    /**
     * @brief Constructor to initialize routing and kinetics model.
     * @param c_dest Destination index for concentrate stream.
     * @param t_dest Destination index for tailings stream.
     * @param c CUnit object representing the kinetic model.
     */
    Unit(int c_dest, int t_dest, const CUnit& c)
        : conc_dest(c_dest), tail_dest(t_dest), cell(c) {}
    /**
     * @brief Default constructor.
     */        
    Unit() = default;
};

/**
 * @brief Simulate the flow of materials through a circuit of processing units.
 *
 * Performs an iterative simulation of material flow through the units, routing outputs
 * to other units or to product/tailings streams, until convergence or maximum iterations.
 *
 * @param units Vector of Unit objects representing the circuit.
 * @param feed_index Index of the unit receiving the external feed.
 * @param out OutputStreams structure to accumulate product and tailings results.
 * @param max_iters Maximum number of iterations to perform (default: 1000).
 * @param tol Convergence tolerance for the simulation (default: 1e-6).
 * @param debug If true, enables verbose output for debugging (default: true).
 */
void simulate_flow(std::vector<Unit>& units,
                   int  feed_index,
                   OutputStreams& out,
                   int  max_iters = 1000,
                   double tol     = 1e-6,
                   bool debug     = true);