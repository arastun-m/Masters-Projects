/**
 * @file simulate_flow.cpp
 * @brief Implements the iterative simulation of material flow in a mineral processing circuit.
 */
#include "simulate_flow.h"
#include "CUnit.h"
#include <CSimulator.h>


// Product arrays for each unit
/**
 * @var unit_feeds
 * @brief Stores the feed stream for each unit after simulation.
 */
std::vector<Stream> unit_feeds;
/**
 * @var unit_concentrates
 * @brief Stores the concentrate stream for each unit after simulation.
 */
std::vector<Stream> unit_concentrates;
/**
 * @var unit_tailings
 * @brief Stores the tailings stream for each unit after simulation.
 */
std::vector<Stream> unit_tailings;

/**
 * @brief Simulates the flow of materials through a circuit of processing units.
 *
 * This function performs an iterative simulation of material flow through a set of units,
 * routing outputs to other units or to product/tailings streams, until convergence is reached
 * or the maximum number of iterations is exceeded.
 *
 * @param units Vector of Unit objects representing the circuit.
 * @param feed_index Index of the unit receiving the external feed.
 * @param out OutputStreams structure to accumulate product and tailings results.
 * @param max_iters Maximum number of iterations to perform.
 * @param tol Convergence tolerance for the simulation.
 * @param debug If true, enables verbose output for debugging.
 */
void simulate_flow(std::vector<Unit>& units,
                   const int  feed_index,
                   OutputStreams &out,
                   const int  max_iters,
                   const double tol,
                   const bool debug)
{
    const int N = static_cast<int>(units.size());

    if (feed_index < 0 || feed_index >= N)
    {
        std::cerr << "[simulate_flow] invalid feed_index\n";
        return;
    }

    // Resize unit product arrays
    unit_feeds.resize(units.size());
    unit_concentrates.resize(units.size());
    unit_tailings.resize(units.size());

    // Index of the three terminal “dummy” nodes that Parse_vector()
    // appends to the end of the units array:
    //   N-3: Palusznium product
    //   N-2: Gormanium  product
    //   N-1: Tailings    product
    const int PAL_OUT = N - 3;
    const int GOR_OUT = N - 2;
    const int TAIL_OUT = N - 1;
    // External feed stream injected at the specified unit on every iteration
    const Stream circuit_feed{8.0, 12.0, 80.0};

    // Step 1 — Initialization: clear all internal streams
    // Only the feed unit receives the initial external feed to start the iteration
    for (auto &u : units)
    {
        u.feed.clear();
        u.conc.clear();
        u.tail.clear();
    }
    units[feed_index].feed = circuit_feed;

    std::vector<Stream> next_feed(N); // Temporary storage for next iteration's feed

    for (int it = 0; it < max_iters; ++it)
    {
        // Compute outputs (concentrate and tailings) for all units
        for (auto &u : units)
        {
            u.cell.calculateOutputs(u.feed.pal, u.feed.gor, u.feed.waste);

            u.conc.pal = u.cell.getPaluszniumConcentrate();
            u.conc.gor = u.cell.getGormaniumConcentrate();
            u.conc.waste = u.cell.getWasteConcentrate();

            u.tail.pal = u.cell.getPaluszniumTailings();
            u.tail.gor = u.cell.getGormaniumTailings();
            u.tail.waste = u.cell.getWasteTailings();
        }

        // Clear next_feed buffer and reset product accumulators
        for (auto& s : next_feed) s.clear();
        out.palusznium1 = 0.0;
        out.gormanium1 = 0.0;
        out.waste1 = 0.0;

        out.palusznium2 = 0.0;
        out.gormanium2 = 0.0;
        out.waste2 = 0.0;

        out.tailings_pal = 0.0;
        out.tailings_gor = 0.0;
        out.tailings_waste = 0.0;

        // Route all outputs to their destinations
        // Can be internal (next unit's feed) or external (products/tailings)
        const auto route = [&](const Stream &s, int dest)
        {
            if (dest >= 0 && dest < PAL_OUT)
            { // Route to another unit
                next_feed[dest] += s;
            }
            else if (dest == PAL_OUT)
            { // Route to Palusznium
                out.palusznium1 += s.pal;
                out.gormanium1 += s.gor;
                out.waste1 += s.waste;
            }
            else if (dest == GOR_OUT)
            { // Route to Gormanium
                out.palusznium2 += s.pal;
                out.gormanium2 += s.gor;
                out.waste2 += s.waste;
            }
            else if (dest == TAIL_OUT)
            { // Route to Tailings
                out.tailings_pal += s.pal;
                out.tailings_gor += s.gor;
                out.tailings_waste += s.waste;
            }
        };


        // Apply routing for both output streams of each unit
        for (int i = 0; i < N; ++i)
        {
            route(units[i].conc, units[i].conc_dest);
            route(units[i].tail, units[i].tail_dest);
        }

        // Inject fixed external feed again at the specified feed unit
        next_feed[feed_index] += circuit_feed;

        // Check convergence and update feeds
        double max_d = 0.0;
        for (int k = 0; k < N; ++k)
        {
            max_d = std::max(max_d, units[k].feed.diff(next_feed[k]));
            units[k].feed = next_feed[k];

            if (debug)
            {
                std::cout << "[Iter " << it << "] next_feed[" << k << "] = ("
                          << next_feed[k].pal << ", " << next_feed[k].gor
                          << ", " << next_feed[k].waste << ")\n";
            }
        }

        if (max_d < tol)
        {
            if (debug)
                std::cout << "[simulate_flow] Converged in " << it + 1 << " iterations.\n";
            // Populate unit product arrays
            for (size_t i = 0; i < units.size(); ++i) {
                unit_feeds[i] = units[i].feed;
                unit_concentrates[i] = units[i].conc;
                unit_tailings[i] = units[i].tail;
            }
            return;
        }
    }

    if (debug)
    {
        std::cerr << "[simulate_flow] WARNING: did not converge in "
                  << max_iters << " iterations.\n";
    }
}