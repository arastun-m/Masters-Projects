/**
 * @file CUnit.h
 * @brief Header for the CUnit class representing a processing unit in the circuit.
 */
#pragma once
#include <algorithm>
#include <cmath>

/**
 * @class CUnit
 * @brief Represents a single processing unit in the mineral processing circuit.
 *
 * Provides methods for setting connections, resetting state, and calculating
 * output streams based on first-order kinetic relationships.
 */
class CUnit {
public:
    //! Index of the unit to which this unit's concentrate stream is connected
    int conc_num;
    
    //! Index of the unit to which this unit's tailings stream is connected
    int tails_num;
    
    //! A Boolean that is changed to true if the unit has been seen
    bool mark;
    
    //! Constructor with default volume
    /**
     * @brief Constructor with default volume.
     * @param volume Initial volume of the unit (m^3).
     */
    CUnit(double volume = 10.0) : m_volume(volume) {
        // Initialize connection indices to invalid values
        conc_num = -1;
        tails_num = -1;
        
        // Initialize mark as not seen
        mark = false;
        
        // Initialize flowrates to zero
        m_paluszniumConcentrate = 0.0;
        m_gormaniumConcentrate = 0.0;
        m_wasteConcentrate = 0.0;
        m_paluszniumTailings = 0.0;
        m_gormaniumTailings = 0.0;
        m_wasteTailings = 0.0;
    }
    /**
     * @brief Constructor with specified concentrate and tailings connections.
     * @param conc Index for concentrate connection.
     * @param tails Index for tailings connection.
     */
    CUnit(int conc, int tails) : conc_num(conc), tails_num(tails), mark(false) {}

    // --- helpers ---------------------------------------------------------
    /**
     * @brief Reset the visited mark flag to false.
     */
    void resetMark() { mark = false; }
    /**
     * @brief Set the concentrate and tailings connections.
     * @param c Index for concentrate connection.
     * @param t Index for tailings connection.
     */
  void setConnections(int c, int t) { conc_num = c; tails_num = t; }
    
    /**
     * @brief Calculate recoveries and flowrates based on input feeds.
     *
     * Implements the first-order kinetic relationship for material recovery:
     * 1. Calculate residence time (τ) = (φ * V) / Σ(Fi/ρ)
     * 2. Calculate recovery rates (Ri) = (ki * τ) / (1 + ki * τ) for each material
     * 3. Calculate output streams
     *
     * @param paluszniumFeed Amount of palusznium in feed (kg/s)
     * @param gormaniumFeed Amount of gormanium in feed (kg/s)
     * @param wasteFeed Amount of waste in feed (kg/s)
     */
    void calculateOutputs(double paluszniumFeed, double gormaniumFeed, double wasteFeed) {
        // Ensure minimum flow rate to prevent division by zero
        paluszniumFeed = std::max(paluszniumFeed, MIN_FLOW_RATE);
        gormaniumFeed = std::max(gormaniumFeed, MIN_FLOW_RATE);
        wasteFeed = std::max(wasteFeed, MIN_FLOW_RATE);
        
        // Calculate volumetric feed rate (sum of Fi/ρ)
        double sumFiOverRho = (paluszniumFeed + gormaniumFeed + wasteFeed) / DENSITY;
        
        // Calculate residence time (τ)
        double tau = (SOLIDS_FRACTION * m_volume) / sumFiOverRho;
        
        // Calculate recoveries to concentrate
        double R_palusznium = (k_palusznium * tau) / (1.0 + k_palusznium * tau);
        double R_gormanium = (k_gormanium * tau) / (1.0 + k_gormanium * tau);
        double R_waste = (k_waste * tau) / (1.0 + k_waste * tau);
        
        // Calculate output flowrates to concentrate
        m_paluszniumConcentrate = paluszniumFeed * R_palusznium;
        m_gormaniumConcentrate = gormaniumFeed * R_gormanium;
        m_wasteConcentrate = wasteFeed * R_waste;
        
        // Calculate tailings as remainder
        m_paluszniumTailings = paluszniumFeed - m_paluszniumConcentrate;
        m_gormaniumTailings = gormaniumFeed - m_gormaniumConcentrate;
        m_wasteTailings = wasteFeed - m_wasteConcentrate;
    }
    
    // Getters for output flowrates
    /**
     * @brief Get the palusznium flowrate in concentrate stream.
     * @return Palusznium concentrate flowrate (kg/s)
     */
    double getPaluszniumConcentrate() const { return m_paluszniumConcentrate; }
    
    /**
     * @brief Get the gormanium flowrate in concentrate stream.
     * @return Gormanium concentrate flowrate (kg/s)
     */
    double getGormaniumConcentrate() const { return m_gormaniumConcentrate; }
    
    /**
     * @brief Get the waste flowrate in concentrate stream.
     * @return Waste concentrate flowrate (kg/s)
     */    
    double getWasteConcentrate() const { return m_wasteConcentrate; }
    
    /**
     * @brief Get the palusznium flowrate in tailings stream.
     * @return Palusznium tailings flowrate (kg/s)
     */    
    double getPaluszniumTailings() const { return m_paluszniumTailings; }

    /**
     * @brief Get the gormanium flowrate in tailings stream.
     * @return Gormanium tailings flowrate (kg/s)
     */    
    double getGormaniumTailings() const { return m_gormaniumTailings; }
    
    /**
     * @brief Get the waste flowrate in tailings stream.
     * @return Waste tailings flowrate (kg/s)
     */    
    double getWasteTailings() const { return m_wasteTailings; }
    
    /**
     * @brief Set the volume of the unit.
     * @param volume The volume in cubic meters.
     */
    void setVolume(double volume) { m_volume = volume; }
    
    /**
     * @brief Get the current volume of the unit.
     * @return The volume in cubic meters.
     */
    double getVolume() const { return m_volume; }
    
private:
    double m_volume;  ///< Cell volume in m^3
    
    // Rate constants for recovery from Appendix
    const double k_palusznium = 0.008;    ///< Palusznium rate constant (s^-1)
    const double k_gormanium = 0.004;     ///< Gormanium rate constant (s^-1)
    const double k_waste = 0.0005;        ///< Waste rate constant (s^-1)
    
    // Output flowrates
    double m_paluszniumConcentrate; ///< Palusznium in concentrate (kg/s)
    double m_gormaniumConcentrate;  ///< Gormanium in concentrate (kg/s)
    double m_wasteConcentrate;      ///< Waste in concentrate (kg/s)
    
    double m_paluszniumTailings;    ///< Palusznium in tailings (kg/s)
    double m_gormaniumTailings;     ///< Gormanium in tailings (kg/s)
    double m_wasteTailings;         ///< Waste in tailings (kg/s)
    
    // Constants
    const double DENSITY = 3000.0;         ///< Density (kg/m^3)
    const double SOLIDS_FRACTION = 0.1;    ///< Solids fraction (10%)
    const double MIN_FLOW_RATE = 1.0e-10;  ///< Minimum flow rate to prevent division by zero
};
