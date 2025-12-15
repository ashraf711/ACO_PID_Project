Below is a **clean, revised `README.md` focused ONLY on ACO**, with all BCO references removed and wording aligned with what you actually implemented and reported.

You can replace your current README with this version directly.

---

# Parallel Ant Colony Optimization (ACO) for PID Controller Tuning

## Overview

This project investigates the use of **Ant Colony Optimization (ACO)** for tuning **PID controller gains** for three linear plant models. Both **serial** and **OpenMP-parallel** implementations are developed and evaluated, with a focus on **execution-time reduction**, **parallel scalability**, and **closed-loop control performance**.

The project compares:

* Baseline hand-tuned PID controllers
* ACO-optimized PID controllers
* Serial vs parallel execution performance of ACO

All simulations, optimization results, plots, and tables included in the final report are reproducible using the provided code and scripts.

---

## Plant Models

The following continuous-time plant models are considered:

* **G₁(s)** = 1 / (s + 1)
* **G₂(s)** = 5 / (s² + 2s + 5)
* **G₃(s)** = 10 / [(s + 1)(s² + 2s + 10)]

Each plant is controlled using a discrete-time PID controller and evaluated using a unit step reference input.

---

## Repository Structure

```
.
├── src/
│   ├── pid_simulator.{h,cpp}     # PID controller + plant simulation
│   ├── utils.{h,cpp}             # Random utilities and helpers
│   ├── aco.{h,cpp}               # Serial ACO implementation
│   ├── aco_parallel.{h,cpp}      # Parallel ACO (OpenMP)
│
├── scripts/
│   └── run_aco.sh                # Automates ACO thread sweep
│
├── data/
│   └── logs/
│       ├── aco_parallel_results.csv
│       └── aco_best.csv
│
├── figures/
│   └── aco_parallel/
│       ├── aco_pid_comparison_G1.pdf
│       ├── aco_pid_comparison_G2.pdf
│       ├── aco_pid_comparison_G3.pdf
│       └── aco_pid_all.pdf
│
├── analysis/
│   └── plot_results.py           # Python analysis & plotting
│
└── README.md
```

---

## Ant Colony Optimization (ACO)

* PID gains ((K_p, K_i, K_d)) are discretized into finite bins
* Each ant probabilistically selects PID parameters based on pheromone trails
* Fitness is evaluated using **Mean Squared Error (MSE)** of the tracking error
* Pheromone updates reinforce parameter combinations yielding lower MSE
* The algorithm iterates until convergence or a maximum iteration count is reached

---

## Parallelization Strategy

* OpenMP is used to parallelize the **fitness evaluation of ants**
* Each thread independently simulates closed-loop plant responses
* Shared data is limited to global-best tracking and pheromone updates
* Thread-private variables are used for random number generation and local fitness

The following parallel performance metrics are computed:

* **Execution Time**
* **Speedup:** ( S(p) = T_1 / T_p )
* **Efficiency:** ( E(p) = S(p) / p )
* **Cost:** ( C(p) = p \cdot T_p )

Thread counts tested: **1, 2, 4, 6, 8, 10**

---

## How to Build

### Requirements

* C++ compiler with OpenMP support (GCC ≥ 9 recommended)
* Python 3.8+ (for analysis and plotting)
* NumPy, Pandas, Matplotlib

### Compile Example

```bash
g++ -O2 -fopenmp aco_parallel.cpp pid_simulator.cpp utils.cpp -o aco_parallel
```

---

## Running Experiments

### Run Parallel ACO Sweep

```bash
chmod +x scripts/run_aco.sh
./scripts/run_aco.sh
```

Results are saved automatically to:

* `data/logs/aco_parallel_results.csv`
* `data/logs/aco_best.csv`

---

## Analysis & Plotting

Python scripts are provided to:

* Compare baseline and ACO-optimized PID step responses
* Compute rise time, settling time, overshoot, peak time, steady-state error, and MSE
* Generate publication-ready PDF figures
* Generate LaTeX-ready tables for the report

Run:

```bash
python analysis/plot_results.py
```

---

## Key Findings

* ACO significantly improves PID performance over baseline tuning for all plant models
* Parallel ACO achieves **over 5× speedup** on 10 threads
* Scaling becomes sublinear beyond 6–8 threads due to synchronization overhead and heterogeneous CPU cores
* Higher-order plants benefit substantially from optimization but may exhibit increased transient overshoot

---

## Notes

* Results were obtained on an Apple M-series processor
* Mixed performance and efficiency cores limit ideal linear scaling
* Random seeds are fixed to ensure reproducibility
