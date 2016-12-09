#!/bin/bash
for i in {1..1}
do
    for filename in ./input/*.txt; do
    	./cmake-build-debug/Coiote_heuristic -i $filename -o ./output/summary.csv -s ./output/solutions/$i.csv

    done
done
