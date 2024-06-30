#!/bin/bash

while true; do
    echo "Loop iteration"
    # PYTHONPATH=. python3 scripts/check_leaderboard.py
    PYTHONPATH="./language/" python scripts/check_leaderboard.py
    date
    sleep 600  # Sleep for 10 minutes
done