#!/bin/bash

echo "=========================================="
echo "Starting EEVDF Scheduler Experiment"
echo "=========================================="
echo "Running 3 processes with nice values: 0, 10, 19"

# Run three processes on the same CPU core 
echo "Starting processes..."
sudo taskset -c 0 ./cpu 1 100 0 &
sudo taskset -c 0 ./cpu 1 100 10 &
sudo taskset -c 0 ./cpu 1 100 19 &

# Wait for all background processes to complete
echo "Waiting for all processes to complete..."
wait

echo ""
echo "=========================================="
echo "Experiment completed!"
echo "=========================================="
echo ""
echo "To view kernel logs, run:"
echo " sudo dmesg | grep EEVDF"
