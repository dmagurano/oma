#!/bin/bash

    for filename in ./input/*.txt; do
    	./cmake-build-debug/Coiote_heuristic -i $filename -o ./output/summary.csv -s ./output/solutions/solution.csv

    done

