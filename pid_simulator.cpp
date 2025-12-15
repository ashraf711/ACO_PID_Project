#include "pid_simulator.h"
#include <cmath>

// Simulates PID control of a continuous-time plant
PIDResult simulatePID(const PIDParams& params,
                      const double* num, int numSize,
                      const double* den, int denSize,
                      double dt, double simTime)
{
    const double MAX_VAL = 1e6;
    const double U_MAX   = 1e3;
    const double I_MAX   = 1e3;

    // PID gains
    double Kp = params.Kp;
    double Ki = params.Ki;
    double Kd = params.Kd;

    // States
    double x1 = 0.0, x2 = 0.0, x3 = 0.0;
    double y  = 0.0;

    double integral = 0.0;
    double prevError = 0.0;

    double mse = 0.0;
    int steps = (int)(simTime / dt);
    const double reference = 1.0;

    for (int k = 0; k < steps; k++) {

        double error = reference - y;

        // PID controller
        integral += error * dt;
        integral = std::fmax(-I_MAX, std::fmin(I_MAX, integral));

        double derivative = (error - prevError) / dt;
        double u = Kp * error + Ki * integral + Kd * derivative;
        u = std::fmax(-U_MAX, std::fmin(U_MAX, u));

        // Bail out on numerical instability
        if (!std::isfinite(u)) {
            mse = 1e9;
            break;
        }

        //  Plant Simulation
        if (denSize == 2) {
            // G1: y' + a y = b u
            double a = den[1];
            double b = num[0];
            y += dt * (-a * y + b * u);
        }
        else if (denSize == 3) {
            // G2: y'' + a1 y' + a0 y = b u
            double a1 = den[1];
            double a0 = den[2];
            double b  = num[0];

            double ydd = b * u - a1 * x2 - a0 * x1;
            x2 += dt * ydd;
            x1 += dt * x2;
            y = x1;
        }
        else if (denSize == 4) {
            // G3: y''' + a2 y'' + a1 y' + a0 y = b u
            double a2 = den[1];
            double a1 = den[2];
            double a0 = den[3];
            double b  = num[0];

            double yddd = b * u - a2 * x3 - a1 * x2 - a0 * x1;
            x3 += dt * yddd;
            x2 += dt * x3;
            x1 += dt * x2;
            y = x1;
        }

        if (!std::isfinite(y) || std::fabs(y) > MAX_VAL) {
            mse = 1e9;
            break;
        }

        mse += error * error;
        prevError = error;
    }

    if (mse < 1e9 && steps > 0) {
        mse /= steps;
    }

    PIDResult result;
    result.mse = mse;
    result.finalValue = y;
    return result;
}
