#!/bin/bash
set -euo pipefail

OUTPUT_FILE="data/logs/aco_parallel_results.csv"
BEST_FILE="data/logs/aco_best.csv"

mkdir -p data/logs


rm -f "$BEST_FILE"
echo "threads,plant,time" > "$OUTPUT_FILE"

THREADS=(1 2 4 6 8 10)
PLANTS=(1 2 3)

for t in "${THREADS[@]}"; do
  for p in "${PLANTS[@]}"; do
    echo "Running: threads=$t plant=G$p"

    OMP_NUM_THREADS=$t ./aco_parallel $p

    # aco_best.csv format: threads,plant,time,bestMSE,Kp,Ki,Kd
    # extract only threads,plant,time:
    tail -n 1 "$BEST_FILE" | cut -d',' -f1-3 >> "$OUTPUT_FILE"

    sleep 0.2
  done
done

echo "---------------------------------------"
echo "All ACO experiments completed!"
echo "Results saved to $OUTPUT_FILE"
echo "Raw best log saved to $BEST_FILE"
echo "---------------------------------------"
