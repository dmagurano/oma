#include <list>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include "utils.h"
#include "heuristic.h"

#include <ga/ga.h>
#define INSTANTIATE_REAL_GENOME
#include <ga/GARealGenome.h>
using namespace std;

/////////////////////////////////////////////
float Objective(GAGenome& g);
void Initializer(GAGenome& g);

int nTimeSteps;
int nCustomerTypes;
int nCells;
Data problem;
int**** solution;

/////////////////////////////////////////

int main(int argc,char *argv[]){
	bool _test = false;
	string _inPath;
	string _outPath;
	string _solPath;

	// Read input parameters
	for(int i=1; i< argc;i++) {
		if(strcmp(argv[i], "-i") == 0)
			_inPath = argv[++i];
		else if(strcmp(argv[i], "-o") == 0)
			_outPath = argv[++i];
		else if(strcmp(argv[i], "-s") == 0)
			_solPath = argv[++i];
		else if(strcmp(argv[i], "-test") == 0)
			_test = true;
	}

	if(!_test && (_inPath.empty() || _outPath.empty())) {
		cout << "------------------------------ " << endl;
		cout << "CMD" << endl;
		cout << "------------------------------ " << endl;
		cout << "-i path of the instance file" << endl;
		cout << "-o path of the output file" << endl;
		cout << "-s path of the output solution file or of the solution to test(optional)" << endl;
		cout << "-test enable the feasibility test (optional)" << endl;
        return 1;
	}

	if(!_test) {
		// Read the instance file
		Heuristic _heuristic(_inPath);
		// Solve the problem
		vector<double> stat;
		//_heuristic.prova(stat);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        nTimeSteps = _heuristic.nTimeSteps;
        nCustomerTypes = _heuristic.nCustomerTypes;
        nCells = _heuristic.nCells;
        problem = _heuristic.problem;
        solution = _heuristic.solution;

        double objFun=0;
        clock_t tStart = clock();

        for (int i = 0; i < nCells; i++)
            for (int j = 0; j < nCells; j++)
                for (int m = 0; m < nCustomerTypes; m++)
                    for (int t = 0; t < nTimeSteps; t++)
                        solution[i][j][m][t] = 0;

        int length = nCells*nCells*nCustomerTypes*nTimeSteps;
        int popsize  = 100;
        int ngen     = 2000;
        float pmut   = 0.3;
        float pcross = 0.7;



       GARealAlleleSetArray alleles;



       for (int i = 0; i < nCells; i++)
           for (int j = 0; j < nCells; j++)
               for (int m = 0; m < nCustomerTypes; m++)
                   for (int t = 0; t < nTimeSteps; t++)
                       alleles.add(0, (float)problem.activities[j], 1 );

      //  GARealAlleleSet alleles(0,30,1,GAAllele::INCLUSIVE,GAAllele::EXCLUSIVE);

        GARealGenome genome(alleles, Objective);     // create a genome
        genome.initializer(Initializer);
        genome.mutator(GARealGenome::FlipMutator);
        genome.crossover(GARealGenome::UniformCrossover);

        GASimpleGA ga(genome);// create the GA
        
        ga.minimize();
        ga.populationSize(popsize);
        ga.nGenerations(ngen);
        ga.pMutation(pmut);
        ga.pCrossover(pcross);

        ga.evolve();
		
		cout << "best individual is " << ga.statistics().bestIndividual() << "\n\n";
		cout << ga.statistics() << "\n";
		

        GARealGenome & result = (GARealGenome &)ga.statistics().bestIndividual();
        for (int i = 0; i < nCells; i++)
            for (int j = 0; j < nCells; j++)
                for (int m = 0; m < nCustomerTypes; m++)
                    for (int t = 0; t < nTimeSteps; t++)
                        objFun +=  result.gene(i + j*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps) * problem.costs[i][j][m][t];

        stat.push_back(objFun);
        stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

        for (int i = 0; i < nCells; i++)
            for (int j = 0; j < nCells; j++)
                for (int m = 0; m < nCustomerTypes; m++)
                    for (int t = 0; t < nTimeSteps; t++)
                        solution[i][j][m][t] =  result.gene(i + j*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps) ;

        _heuristic.hasSolution = true;

        /////////////////////////////////////////////////////////////////

		_heuristic.getStatSolution(stat);
		// Write KPI of solution
        string instanceName = splitpath(_inPath);
        _heuristic.writeKPI(_outPath, instanceName, stat);
		// Write solution
		if(!_solPath.empty())
			_heuristic.writeSolution(_solPath);

	}
	else {
        // Read the instance file
		Heuristic _heuristic(_inPath);
		// Read the solution file
		eFeasibleState _feasibility = _heuristic.isFeasible(_solPath);
		switch(_feasibility) {
			case FEASIBLE:
				cout << "Solution is feasible" << endl;
				break;
			case NOT_FEASIBLE_DEMAND:
				cout << "Solution is not feasible: demand not satisfied" << endl;
				break;
			case NOT_FEASIBLE_USERS:
				cout << "Solution is not feasible: exceeded number of available users" << endl;
				break;
		}
	}

	return 0;
}

float Objective(GAGenome& g){
    GARealGenome & genome = (GARealGenome &)g;

    float score = 0.0;

    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    score +=  genome.gene(i + j*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps) * problem.costs[i][j][m][t];

    // Demand
    bool feasible = true;
    int expr;
    for (int i = 0; i < nCells; i++) {
        expr = 0;
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    expr += problem.n[m] * genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps);
        if (expr < problem.activities[i])
            feasible = false;
    }

    // Max Number of users
    for (int i = 0; i < nCells; i++)
        for (int m = 0; m < nCustomerTypes; m++)
            for (int t = 0; t < nTimeSteps; t++) {
                expr = 0;
                for (int j = 0; j < nCells; j++)
                    expr += genome.gene(i + j*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps);
                if (expr > problem.usersCell[i][m][t])
                    feasible = false;
            }

    if(!feasible)
        return 100000000;
	
	


    return score ;

}

//void Initializer(GAGenome& g)
//{
//    GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int>&)g;
//    for (int i = 0; i < nCells; i++)
//        for (int j = 0; j < nCells; j++)
//            for (int m = 0; m < nCustomerTypes; m++)
//                for (int t = 0; t < nTimeSteps; t++)
//                    genome.gene(i + j*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps,  GARandomInt(0, problem.activities[j]) ) ;
//
//}

void Initializer(GAGenome& g)
{
    GARealGenome & genome = (GARealGenome &)g;

    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    genome.gene(i + j*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps,  0 ) ;

    for (int i = 0; i < nCells; i++) {
        int demand = problem.activities[i];

        bool notSatisfied = true;

        while (notSatisfied) {
            int j = GARandomInt(0, nCells-1);
            int m = GARandomInt(0, nCustomerTypes-1);
            int t = GARandomInt(0, nTimeSteps-1);

            if (i != j) {
                if (demand > problem.n[m] * problem.usersCell[j][m][t]) {
                    genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps, problem.usersCell[j][m][t]);
                    problem.usersCell[j][m][t] -= genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps);
                }
                else {
                    float saved = genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps);
                    genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps,  saved + floor(demand / problem.n[m]));
                    notSatisfied = false;
                }
                if (genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps) != 0) {
                    //objFun += solution[j][i][m][t] * problem.costs[j][i][m][t];
                }
                demand -= problem.n[m]*genome.gene(j + i*nCells + m*nCells*nCustomerTypes + t*nCells*nCustomerTypes*nTimeSteps);
            }

        }
    }


}
