#ifndef ACO_PARALLEL_H
#define ACO_PARALLEL_H

#include "aco.h"

// Parallel ACO using OpenMP.
// Same interface as runACO, but with internal parallel loops.
void runACOParallel(const double* num, int numSize,
                    const double* den, int denSize,
                    const ACOSettings& settings,
                    PIDParams& bestParams,
                    double& bestMSE,
                    const char* logFilePath);

#endif // ACO_PARALLEL_H
