#!/bin/bash
#script to check feasibility and gap between optimal solution
opt_file="optimal_solutions.csv"
echo "INSTANCE OPT HEUR GAP" >> randomica.csv
source_dir=input
if [ $(ls input2/ | wc -c) -gt 0 ]
then
	source_dir=input2
fi
avg_file="temp_average_file"

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
	
	echo $heur_sol $opt_sol
	
	opt_gap=$(python -c "print str(round((float($heur_sol) - float(str($opt_sol).lstrip('\t')))/float($opt_sol) * 100.0,2)).replace('.', ',')")
	
	echo $opt_gap >> $avg_file
	
	echo "Optimality gap: $opt_gap%"
	

	echo "$filename;$opt_sol;$heur_sol;$opt_gap" >> randomica.csv
	((i++))
done

echo "Average gap: "$(python -c "f=open('$avg_file'); print round(sum([float(line.rstrip('\\n').replace(',', '.')) for line in f])/$i, 2); f.close()")
rm $avg_file
echo "Test completati!"