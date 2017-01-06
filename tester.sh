#!/bin/bash
#script to check feasibility and gap between optimal solution
opt_file="optimal_solutions.csv"
out_file="./output/test_results.csv"
echo "INSTANCE OPT HEUR GAP" >> $out_file
source_dir=input
if [ $(ls input2/ | wc -c) -gt 0 ]
then
	source_dir=input2
fi
avg_file="temp_average_file"
non_optimal_count=0

touch $avg_file
cat /dev/null > $avg_file

i=0
for input_file in ./$source_dir/*.txt; do
	filename=$(echo $input_file | cut -f3 -d/)
	echo -n "$input_file: " 
	./Coiote -i $input_file -o ./output/summary.csv -s ./output/$filename.csv > /dev/null
	
	#feasibility check
	
	echo $(./Coiote -i $input_file -s ./output/$filename.csv -test)

	
	heur_sol=$(tail -1 ./output/summary.csv | cut -f2 -d';')
	
	opt_sol=$(cat $opt_file | grep -w $(echo $filename | cut -f1 -d.) | cut -f3 -d';' | cut -c2-)
	
	opt_gap=$(python gap.py $heur_sol $opt_sol)
	if [ $? -eq 1 ]
	then
		((non_optimal_count++))
	fi
	
	echo $opt_gap >> $avg_file
	
	echo "Heuristic Optimal Gap: $heur_sol $opt_sol $opt_gap%"
	

	echo "$filename;$opt_sol;$heur_sol;$opt_gap" >> $out_file
	((i++))
done

echo "Average gap: "$(python -c "f=open('$avg_file'); print round(sum([float(line.rstrip('\\n').replace(',', '.')) for line in f])/$i, 2); f.close()")
if [ $non_optimal_count -gt 0 ]
then
	echo "Numero di soluzioni sopra il 2 percento: $non_optimal_count"
fi
rm $avg_file
echo "Test completati!"