#!/bin/bash
for filename in ./input2/*.txt; do
	./Coiote -i $filename -o ./output/summary.csv
done