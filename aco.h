#ifndef ACO_H
#define ACO_H

#include "pid_simulator.h"

// One ant = one PID candidate
struct Ant {
    PIDParams pid;      // Kp, Ki, Kd
    double cost;        // MSE (lower is better)
    int kpBin, kiBin, kdBin;  // chosen discretized bins (for pheromone update)
};

// Settings for ACO algorithm
struct ACOSettings {
    int numAnts;        // number of ants per iteration
    int maxIterations;  // number of iterations

    // ACO hyperparameters
    double alpha;       // pheromone influence
    double rho;         // evaporation rate in (0,1)
    double Q;           // deposit factor

    // Discretization bins (resolution)
    int kpBins;
    int kiBins;
    int kdBins;

    // PID search ranges
    double KpMin, KpMax;
    double KiMin, KiMax;
    double KdMin, KdMax;

    // Simulation settings (passed to simulator)
    double dt;
    double simTime;
};

// Runs Serial ACO for a single plant (num/den).
void runACO(const double* num, int numSize,
            const double* den, int denSize,
            const ACOSettings& settings,
            PIDParams& bestParams,
            double& bestMSE,
            const char* logFilePath);

#endif // ACO_H
