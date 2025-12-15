#include "aco_parallel.h"
#include "utils.h"
#include <vector>
#include <fstream>
#include <cmath>
#include <omp.h>

using namespace std;

static inline double binCenter(int idx, int bins, double minV, double maxV) {
    double width = (maxV - minV) / (double)bins;
    return minV + (idx + 0.5) * width;
}

static int rouletteSelect(const vector<double>& w, double sumW) {
    double r = randomDouble(0.0, sumW);
    double acc = 0.0;
    for (int i = 0; i < (int)w.size(); i++) {
        acc += w[i];
        if (acc >= r) return i;
    }
    return (int)w.size() - 1;
}

static double evaluateAnt(Ant& ant,
                          const double* num, int numSize,
                          const double* den, int denSize,
                          double dt, double simTime)
{
    PIDResult result = simulatePID(ant.pid, num, numSize, den, denSize, dt, simTime);
    return result.mse;
}

static void logToCSV(ofstream& out, int iteration, const PIDParams& best, double bestMSE) {
    out << iteration << ","
        << bestMSE << ","
        << best.Kp << ","
        << best.Ki << ","
        << best.Kd << "\n";
}

void runACOParallel(const double* num, int numSize,
                    const double* den, int denSize,
                    const ACOSettings& settings,
                    PIDParams& bestParams,
                    double& bestMSE,
                    const char* logFilePath)
{
    initRandom(12345);

    vector<double> tauP(settings.kpBins, 1.0);
    vector<double> tauI(settings.kiBins, 1.0);
    vector<double> tauD(settings.kdBins, 1.0);

    ofstream logFile;
    if (logFilePath != nullptr) {
        logFile.open(logFilePath);
        if (logFile.is_open()) {
            logFile << "iteration,bestMSE,Kp,Ki,Kd\n";
        }
    }

    bestMSE = 1e9;
    bestParams = {0.0, 0.0, 0.0};

    vector<Ant> ants(settings.numAnts);

    for (int iter = 0; iter < settings.maxIterations; iter++) {

        // weights
        vector<double> wP(settings.kpBins), wI(settings.kiBins), wD(settings.kdBins);
        double sumWP = 0.0, sumWI = 0.0, sumWD = 0.0;

        for (int j = 0; j < settings.kpBins; j++) { wP[j] = pow(tauP[j], settings.alpha); sumWP += wP[j]; }
        for (int j = 0; j < settings.kiBins; j++) { wI[j] = pow(tauI[j], settings.alpha); sumWI += wI[j]; }
        for (int j = 0; j < settings.kdBins; j++) { wD[j] = pow(tauD[j], settings.alpha); sumWD += wD[j]; }

        // Construct + evaluate ants (parallel)
        #pragma omp parallel for
        for (int a = 0; a < settings.numAnts; a++) {
            Ant ant;

            ant.kpBin = rouletteSelect(wP, sumWP);
            ant.kiBin = rouletteSelect(wI, sumWI);
            ant.kdBin = rouletteSelect(wD, sumWD);

            ant.pid.Kp = binCenter(ant.kpBin, settings.kpBins, settings.KpMin, settings.KpMax);
            ant.pid.Ki = binCenter(ant.kiBin, settings.kiBins, settings.KiMin, settings.KiMax);
            ant.pid.Kd = binCenter(ant.kdBin, settings.kdBins, settings.KdMin, settings.KdMax);

            ant.cost = evaluateAnt(ant, num, numSize, den, denSize, settings.dt, settings.simTime);

            ants[a] = ant;
        }

        // Find best ant (serial scan: simple + correct)
        int bestIdxIter = 0;
        for (int a = 1; a < settings.numAnts; a++) {
            if (ants[a].cost < ants[bestIdxIter].cost) bestIdxIter = a;
        }

        // Update global best
        if (ants[bestIdxIter].cost < bestMSE) {
            bestMSE = ants[bestIdxIter].cost;
            bestParams = ants[bestIdxIter].pid;
        }

        // Evaporation
        for (double& t : tauP) t *= (1.0 - settings.rho);
        for (double& t : tauI) t *= (1.0 - settings.rho);
        for (double& t : tauD) t *= (1.0 - settings.rho);

        // Deposit from best ant of iteration
        double deposit = settings.Q / (ants[bestIdxIter].cost + 1e-12);
        tauP[ants[bestIdxIter].kpBin] += deposit;
        tauI[ants[bestIdxIter].kiBin] += deposit;
        tauD[ants[bestIdxIter].kdBin] += deposit;

        if (logFile.is_open()) logToCSV(logFile, iter, bestParams, bestMSE);
    }

    if (logFile.is_open()) logFile.close();
}
