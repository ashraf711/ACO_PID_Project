#include "utils.h"
#include <random>
#include <omp.h>

using namespace std;

static int GLOBAL_SEED = 12345;

thread_local mt19937 rng;
thread_local bool rng_initialized = false;

static inline void ensure_rng() {
    if (!rng_initialized) {
        rng.seed(GLOBAL_SEED + omp_get_thread_num());
        rng_initialized = true;
    }
}

void initRandom(int baseSeed) {
    GLOBAL_SEED = baseSeed;
    rng_initialized = false; // force reseed per thread
}

double randomDouble(double min, double max) {
    ensure_rng();
    uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

int randomInt(int min, int max) {
    ensure_rng();
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

double clamp(double value, double minVal, double maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}
