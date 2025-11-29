#include "simulate_flow.h"
#include "CCircuit.h"
#include <iostream>

int main() {
    int num_units = 4;
    int PAL_IDX   = num_units;
    int GOR_IDX   = num_units + 1;
    int TAIL_IDX  = num_units + 2;

    std::vector<int> circuit_vec = {
        4,
        PAL_IDX, 1,
        2, TAIL_IDX,
        3, 0,
        GOR_IDX, TAIL_IDX
    };

    // Parse circuit structure
    Circuit circ(num_units);
    circ.Parse_vector(circuit_vec);

    // Convert parsed units into Unit objects compatible with simulate_flow
    const size_t N_sim = circ.units.size();
    std::vector<Unit> sim_units;
    sim_units.reserve(N_sim);
    for (size_t i = 0; i < N_sim; ++i) {
        sim_units.emplace_back(
            circ.units[i].conc_num,
            circ.units[i].tails_num,
            CUnit(circ.units[i])
        );
    }

    // Create output holder
    OutputStreams out;

    // Run the simulation
    simulate_flow(sim_units,
                  /*feed_index=*/0,
                  out,
                  /*max_iters=*/1000,
                  /*tol=*/1e-6,
                  /*debug=*/true);

    // Print the results
    std::cout << "\n=== Final Product Flows ===\n";
    std::cout << "Palusznium product: "
              << out.palusznium1 << " pal, "
              << out.gormanium1  << " gor, "
              << out.waste1      << " waste\n";

    std::cout << "Gormanium product:  "
              << out.palusznium2 << " pal, "
              << out.gormanium2  << " gor, "
              << out.waste2      << " waste\n";

    std::cout << "Tailings:           "
              << out.tailings_pal   << " pal, "
              << out.tailings_gor   << " gor, "
              << out.tailings_waste << " waste\n";

    return 0;
}