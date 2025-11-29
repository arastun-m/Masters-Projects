/**
 * @file post_process.cpp
 * @brief Implements functions for exporting simulation results and performance metrics to JSON files.
 */
#include "post_process.h"

/**
 * @brief Exports unit stream data to a JSON file for visualization.
 *
 * This function writes the feed, concentrate, and tailings streams for each processing unit
 * (excluding the last three dummy units) to a JSON file at "plotting/data/unit_data.json".
 *
 * @param unit_feeds Vector of feed streams for each unit.
 * @param unit_concentrates Vector of concentrate streams for each unit.
 * @param unit_tailings Vector of tailings streams for each unit.
 */
void export_unit_data(std::vector<Stream>& unit_feeds, 
    std::vector<Stream>& unit_concentrates, 
    std::vector<Stream>& unit_tailings) {

    json output;
    
    // Create array for all units
    output["units"] = json::array();
    
    // Write data for each unit
    for (size_t i = 0; i < unit_feeds.size()-3; i++) {
        json unit;
        unit["unit_number"] = i;
        
        // Feed streams
        unit["feed"] = {
            {"pal", unit_feeds[i].pal},
            {"gor", unit_feeds[i].gor},
            {"waste", unit_feeds[i].waste}
        };
        
        // Concentrate streams
        unit["concentrate"] = {
            {"pal", unit_concentrates[i].pal},
            {"gor", unit_concentrates[i].gor},
            {"waste", unit_concentrates[i].waste}
        };
        
        // Tailings streams
        unit["tailings"] = {
            {"pal", unit_tailings[i].pal},
            {"gor", unit_tailings[i].gor},
            {"waste", unit_tailings[i].waste}
        };
        
        output["units"].push_back(unit);
    }
  
    std::ofstream outfile("plotting/data/unit_data.json");
    outfile << output.dump(4);
    outfile.close();
}
/**
 * @brief Exports circuit performance data and metrics to a JSON file.
 *
 * This function writes the circuit vector, feed composition, profit, recoveries, and grades
 * to a JSON file at "plotting/data/performance_data.json" for further analysis or plotting.
 *
 * @param num_units Number of processing units in the circuit.
 * @param circuit_vector Pointer to the circuit vector.
 * @param feed Feed stream composition.
 * @param profit Calculated profit for the circuit.
 * @param palusznium_product Array containing Palusznium product stream values.
 * @param gormanium_product Array containing Gormanium product stream values.
 * @param tailings_product Array containing tailings stream values.
 */
void export_performance_data(int num_units,
    int* circuit_vector,
    Stream feed,
    double profit,
    double* palusznium_product,
    double* gormanium_product, 
    double* tailings_product) {
    
    double vector_length = num_units*2 + 1;

    // Calculate Recoveries
    double palusznium_recovery = palusznium_product[0] / feed.pal;
    double gormanium_recovery = gormanium_product[1] / feed.gor;

    // Calculate Grades 
    double palusznium_grade = palusznium_product[0] / (palusznium_product[0] + palusznium_product[1] + palusznium_product[2]);
    double gormanium_grade = gormanium_product[1] / (gormanium_product[0] + gormanium_product[1] + gormanium_product[2]);

    // Create JSON object
    json output;
    
    // Add circuit vector
    output["circuit_vector"] = json::array();
    for (int i = 0; i < vector_length; i++) {
        output["circuit_vector"].push_back(circuit_vector[i]);
    }
    
    // Add feed data
    output["feed"] = json::array();
    output["feed"].push_back(feed.pal);
    output["feed"].push_back(feed.gor);
    output["feed"].push_back(feed.waste);
    
    // Add performance metrics
    output["profit"] = profit;
    output["recoveries"] = {
        {"palusznium", palusznium_recovery},
        {"gormanium", gormanium_recovery}
    };
    output["grades"] = {
        {"palusznium", palusznium_grade},
        {"gormanium", gormanium_grade}
    };

    std::ofstream outfile("plotting/data/performance_data.json");
    outfile << output.dump(4);
    outfile.close();
}
