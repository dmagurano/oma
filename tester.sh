#!/bin/bash
#script to check feasibility and gap between optimal solution
opt_file="optimal_solutions.csv"
	echo "INSTANCE OPT HEUR GAP" >> randomica.csv
for input_file in ./input/*.txt; do
	filename=$(echo $input_file | cut -f3 -d/)
	./Coiote -i $input_file -o ./output/summary.csv -s ./output/$filename.csv > /dev/null
	
	#feasibility check
	echo -n "$input_file: " 
	echo $(./Coiote -i $input_file -s ./output/$filename.csv -test)

	
	heur_sol=$(tail -1 ./output/summary.csv | cut -f3 -d';')
	
	opt_sol=$(cat $opt_file | grep -w $(echo $filename | cut -f1 -d.) | cut -f3 -d';' | cut -c2-)
	
	echo $heur_sol $opt_sol
	
	opt_gap=$(python -c "print round((float($heur_sol) - float(str($opt_sol).lstrip('\t')))/float($opt_sol) * 100.0,2)")
	
	echo "Optimality gap: $opt_gap%"
	

	echo "$filename;$opt_sol;$heur_sol;$opt_gap" >> randomica.csv
done