#ifndef PID_SIMULATOR_H
#define PID_SIMULATOR_H

// Struct to hold PID parameters
struct PIDParams {
    double Kp;
    double Ki;
    double Kd;
};

// Result structure for simulation output
struct PIDResult {
    double mse;         // mean squared error
    double finalValue;  // final plant output
};

// Simulates a PID controller on a continuous-time plant
// defined by numerator/denominator coefficients.
// num, den: transfer function coefficients (highest power first)
// dt: simulation time step
// simTime: total simulation time
PIDResult simulatePID(const PIDParams& params,
                      const double* num, int numSize,
                      const double* den, int denSize,
                      double dt, double simTime);

#endif // PID_SIMULATOR_H
