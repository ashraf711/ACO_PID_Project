#include <iostream>
#include <vector>
#include "aco.h"
#include "utils.h"

using namespace std;

// Plants 
void getG1(vector<double>& num, vector<double>& den) {
    num = {1.0};
    den = {1.0, 1.0};
}
void getG2(vector<double>& num, vector<double>& den) {
    num = {5.0};
    den = {1.0, 2.0, 5.0};
}
void getG3(vector<double>& num, vector<double>& den) {
    num = {10.0};
    den = {1.0, 3.0, 12.0, 10.0};
}

int main() {
    initRandom(12345);

    vector<double> num, den;
    int choice = 2;

    cout << "Choose plant:\n";
    cout << "1. G1(s) = 1/(s+1)\n";
    cout << "2. G2(s) = 5/(s^2+2s+5)\n";
    cout << "3. G3(s) = 10/((s+1)(s^2+2s+10))\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) getG1(num, den);
    else if (choice == 2) getG2(num, den);
    else getG3(num, den);

    ACOSettings settings;
    settings.numAnts = 200;
    settings.maxIterations = 400;

    settings.alpha = 1.0;
    settings.rho   = 0.10;
    settings.Q     = 1.0;

    // Binning resolution
    settings.kpBins = 60;
    settings.kiBins = 60;
    settings.kdBins = 60;

    // PID parameter ranges (start same as your BCO if you want)
    settings.KpMin = -10.0; settings.KpMax = 10.0;
    settings.KiMin = -10.0; settings.KiMax = 10.0;
    settings.KdMin = -10.0; settings.KdMax = 10.0;

    settings.dt = 0.001;
    settings.simTime = 40.0;

    PIDParams bestPID;
    double bestMSE = 1e9;

    const char* logFile;
    if (choice == 1) logFile = "data/logs/aco_G1_serial.csv";
    else if (choice == 2) logFile = "data/logs/aco_G2_serial.csv";
    else logFile = "data/logs/aco_G3_serial.csv";

    cout << "\nRunning Serial ACO Optimization...\n";

    runACO(num.data(), (int)num.size(),
           den.data(), (int)den.size(),
           settings,
           bestPID,
           bestMSE,
           logFile);

    cout << "\n===== ACO Optimization Result =====\n";
    cout << "Best MSE: " << bestMSE << "\n";
    cout << "Best Kp: " << bestPID.Kp << "\n";
    cout << "Best Ki: " << bestPID.Ki << "\n";
    cout << "Best Kd: " << bestPID.Kd << "\n";
    cout << "Log saved to: " << logFile << "\n";

    return 0;
}
