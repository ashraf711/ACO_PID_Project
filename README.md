# Parallel Meta-Heuristic PID Optimization (BCO & ACO)

## Overview

This project investigates the use of **meta-heuristic optimization algorithms**—**Bee Colony Optimization (BCO)** and **Ant Colony Optimization (ACO)**—for tuning **PID controller gains** for three linear plant models. Both **serial** and **OpenMP-parallel** implementations are developed and evaluated, with a focus on **performance scalability**, **solution quality**, and **controller behavior**.

The project compares:

* Baseline hand-tuned PID controllers
* BCO-optimized PID controllers
* ACO-optimized PID controllers
* Serial vs parallel execution performance

All simulations, optimizations, plots, and tables included in the final report are reproducible using the provided code and scripts.

---

## Plant Models

The following continuous-time plant models are considered:

* **G₁(s)** = 1 / (s + 1)
* **G₂(s)** = 5 / (s² + 2s + 5)
* **G₃(s)** = 10 / [(s + 1)(s² + 2s + 10)]

Each plant is controlled using a discrete-time PID controller and evaluated using a step reference input.

---

## Repository Structure

```
.
├── src/
│   ├── pid_simulator.{h,cpp}     # PID controller + plant simulation
│   ├── utils.{h,cpp}             # Random utilities and helpers
│   ├── bco.{h,cpp}               # Serial BCO implementation
│   ├── bco_parallel.{h,cpp}      # Parallel BCO (OpenMP)
│   ├── aco.{h,cpp}               # Serial ACO implementation
│   ├── aco_parallel.{h,cpp}      # Parallel ACO (OpenMP)
│
├── scripts/
│   ├── run_bco.sh                # Automates BCO thread sweep
│   ├── run_aco.sh                # Automates ACO thread sweep
│
├── data/
│   └── logs/
│       ├── bco_parallel_results.csv
│       ├── bco_best.csv
│       ├── aco_parallel_results.csv
│       └── aco_best.csv
│
├── figures/
│   ├── bco_parallel/
│   │   ├── execution_time.pdf
│   │   ├── speedup.pdf
│   │   ├── efficiency.pdf
│   │   └── cost.pdf
│   ├── aco_parallel/
│   │   ├── aco_pid_comparison_G1.pdf
│   │   ├── aco_pid_comparison_G2.pdf
│   │   ├── aco_pid_comparison_G3.pdf
│   │   └── aco_pid_all.pdf
│
├── analysis/
│   └── plot_results.py            # Python analysis & plotting
│
└── README.md
```

---

## Optimization Algorithms

### Bee Colony Optimization (BCO)

* Uses employed, onlooker, and scout bee phases
* PID gains encoded as candidate food sources
* Fitness evaluated using Mean Squared Error (MSE)
* Parallelized using OpenMP across bees

### Ant Colony Optimization (ACO)

* Discretizes PID parameter space into bins
* Ants probabilistically select gains based on pheromone trails
* Global pheromone updates reinforce high-quality solutions
* Parallelized using OpenMP across ants

---

## Parallelization Strategy

* OpenMP is used to parallelize candidate solution evaluation
* Each thread evaluates independent PID simulations
* Parallel performance metrics computed:

  * Execution Time
  * Speedup: ( S(p) = T_1 / T_p )
  * Efficiency: ( E(p) = S(p) / p )
  * Cost: ( C(p) = p \cdot T_p )

Thread counts tested: **1, 2, 4, 6, 8, 10**

---

## How to Build

### Requirements

* C++ compiler with OpenMP support (GCC ≥ 9 recommended)
* Python 3.8+ (for analysis & plotting)
* NumPy, Pandas, Matplotlib

### Compile Example

```bash
g++ -O2 -fopenmp aco_parallel.cpp pid_simulator.cpp utils.cpp -o aco_parallel
g++ -O2 -fopenmp bco_parallel.cpp pid_simulator.cpp utils.cpp -o bco_parallel
```

---

## Running Experiments

### Run ACO Parallel Sweep

```bash
chmod +x scripts/run_aco.sh
./scripts/run_aco.sh
```

### Run BCO Parallel Sweep

```bash
chmod +x scripts/run_bco.sh
./scripts/run_bco.sh
```

Results are saved automatically to:

* `data/logs/aco_parallel_results.csv`
* `data/logs/aco_best.csv`
* `data/logs/bco_parallel_results.csv`
* `data/logs/bco_best.csv`

---

## Analysis & Plotting

Python scripts are provided to:

* Compare baseline vs optimized PID responses
* Compute rise time, settling time, overshoot, MSE, and peak time
* Generate publication-ready PDF figures
* Generate LaTeX-ready tables

Run:

```bash
python analysis/plot_results.py
```

---

## Key Findings

* Both BCO and ACO significantly improve PID performance over baseline tuning
* Parallel implementations achieve **>5× speedup** on 10 threads
* ACO and BCO show diminishing returns beyond 6–8 threads due to hardware limits
* Higher-order plants benefit most from meta-heuristic tuning but may introduce overshoot

---

## Notes

* Results were obtained on an Apple M-series processor
* Due to mixed performance and efficiency cores, perfect linear scaling is not expected
* Random seeds are fixed for reproducibility

