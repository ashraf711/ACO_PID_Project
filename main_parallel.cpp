#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdlib>    
#include <omp.h>

#include "aco_parallel.h"
#include "utils.h"

using namespace std;

static void getG1(vector<double>& num, vector<double>& den) {
    num = {1.0};
    den = {1.0, 1.0};
}
static void getG2(vector<double>& num, vector<double>& den) {
    num = {5.0};
    den = {1.0, 2.0, 5.0};
}
static void getG3(vector<double>& num, vector<double>& den) {
    num = {10.0};
    den = {1.0, 3.0, 12.0, 10.0};
}

int main(int argc, char** argv) {
    initRandom(12345);

    // Plant selection 
    int choice = 2; // 
    if (argc >= 2) choice = atoi(argv[1]);

    if (choice < 1 || choice > 3) {
        cerr << "Usage: " << argv[0] << " <plant>\n"
             << "  plant=1 (G1), 2 (G2), 3 (G3)\n";
        return 1;
    }

    vector<double> num, den;
    if (choice == 1) getG1(num, den);
    else if (choice == 2) getG2(num, den);
    else getG3(num, den);

    // ACO settings 
    ACOSettings settings;
    settings.numAnts = 200;
    settings.maxIterations = 400;

    settings.alpha = 1.0;
    settings.rho   = 0.10;
    settings.Q     = 1.0;

    settings.kpBins = 60;
    settings.kiBins = 60;
    settings.kdBins = 60;

    settings.KpMin = -10.0; settings.KpMax = 10.0;
    settings.KiMin = -10.0; settings.KiMax = 10.0;
    settings.KdMin = -10.0; settings.KdMax = 10.0;

    settings.dt = 0.001;
    settings.simTime = 40.0;

    PIDParams bestPID;
    double bestMSE = 1e9;

    
    std::filesystem::create_directories("data/logs");

    // Timing 
    int threads = omp_get_max_threads();
    std::string logPath =
    (choice == 1) ? "data/logs/aco_G1_T" + std::to_string(threads) + ".csv" :
    (choice == 2) ? "data/logs/aco_G2_T" + std::to_string(threads) + ".csv" :
                    "data/logs/aco_G3_T" + std::to_string(threads) + ".csv";
    const char* logFile = logPath.c_str();
    double t0 = omp_get_wtime();

    runACOParallel(num.data(), (int)num.size(),
                   den.data(), (int)den.size(),
                   settings,
                   bestPID,
                   bestMSE,
                   logFile);

    double elapsed = omp_get_wtime() - t0;

    //  Append runtime CSV 
    const char* bestCsv = "data/logs/aco_best.csv";
    bool writeHeader2 = !std::filesystem::exists(bestCsv);

    std::ofstream bout(bestCsv, std::ios::app);
    if (bout.is_open()) {
        if (writeHeader2) {
            bout << "threads,plant,time,bestMSE,Kp,Ki,Kd\n";
        }
        bout << threads << "," << choice << "," << elapsed << ","
            << bestMSE << ","
            << bestPID.Kp << ","
            << bestPID.Ki << ","
            << bestPID.Kd << "\n";
    }

    // Console summary 
    cout << "Threads: " << threads << "\n";
    cout << "Plant: " << choice << "\n";
    cout << "Runtime(s): " << elapsed << "\n";
    cout << "BestMSE: " << bestMSE << "\n";
    cout << "BestPID: Kp=" << bestPID.Kp
         << " Ki=" << bestPID.Ki
         << " Kd=" << bestPID.Kd << "\n";
    cout << "Convergence log: " << logFile << "\n";
    cout << "Timing log: " << bestCsv << "\n";

    return 0;
}


