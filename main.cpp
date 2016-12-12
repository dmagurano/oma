#include <list>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <chrono>
#include "utils.h"
#include "heuristic.h"

#include "ga/ga.h"


#define cout STD_COUT
#define ostream STD_OSTREAM

using namespace std;

/////////////////////////////////////////////
float Objective(GAGenome& g);
void Initializer(GAGenome& g);

Heuristic _heuristic;
int nTimeSteps;
int nCustomerTypes;
int nCells;
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
		_heuristic = Heuristic(_inPath);
		// Solve the problem
		vector<double> stat;


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        nTimeSteps = _heuristic.nTimeSteps;
        nCustomerTypes = _heuristic.nCustomerTypes;
        nCells = _heuristic.nCells;
        solution = _heuristic.solution;

        int popsize  = 100;
        int ngen     = 1000000;
        float pmut   = 0.01;
        float pcross = 0.9;


        GAListGenome<int> genome(Objective);
        genome.initializer(Initializer);
        genome.mutator(GAListGenome<int>::SwapMutator);
        genome.crossover(GAListGenome<int>::PartialMatchCrossover);


//        GASimpleGA ga(genome);// create the GA
//
//
//        ga.populationSize(popsize);
//        ga.nGenerations(ngen);
//        ga.pMutation(pmut);
//        ga.pCrossover(pcross);

        GASteadyStateGA ga(genome);

        GASigmaTruncationScaling trunc;
        ga.scaling(trunc);
        ga.set(gaNpopulationSize, 20);
        ga.set(gaNpCrossover, 0.6);
        ga.set(gaNpMutation, 0.1);
        ga.set(gaNnGenerations, 10000);
        ga.set(gaNpReplacement, 0.1);


        ga.minimize();



        ga.initialize();

        clock_t tStart = clock();

        while( ((double)(clock() - tStart) / CLOCKS_PER_SEC ) < 5.0 ){
            ga.step();
        }

        stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

		cout << "best individual is " << ga.statistics().bestIndividual() << "\n\n";
		cout << ga.statistics() << "\n";

//        for(int i=0; i<ga.statistics().bestPopulation().size(); i++){
//                genome = ga.statistics().bestPopulation().individual(i);
//                cout << genome << "\n";
//        }

        int objFun = ga.statistics().bestIndividual().score();

        stat.push_back(objFun);

        Data problem = _heuristic.getProblem();
        GAListGenome<int>  & result = (GAListGenome<int>  &)ga.statistics().bestIndividual();

        GAListIter<int> iter(result);
        vector<int> order;
        int *cur, *head;
        if((head=iter.head()) != NULL) order.push_back(*head);
        for(cur=iter.next(); cur && cur != head; cur=iter.next())
           order.push_back(*cur);

        std::vector<int>::iterator it = order.begin();
        std::vector<int>::iterator end = order.end();
        // per ogni dest:
        for (; it!=end; it++)
        {
            cout << *it << " " ;
        }

        cout << "\n";

        _heuristic.solveGreedy(stat,order,problem);

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

    GAListGenome<int> & genome = (GAListGenome<int> &)g;

    auto a = chrono::high_resolution_clock::now().time_since_epoch();
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(a);
    srand(now_ms.count());


    int i, j, m, t, w;
    bool notSatisfied;
    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    solution[i][j][m][t] = 0;

    int  objfun = 0;
    bool feasible = true;

    Data problem = _heuristic.getProblem();

    GAListIter<int> iter(genome);
    vector<int> indexes;
    int *cur, *head;
    if((head=iter.head()) != NULL) indexes.push_back(*head);
    for(cur=iter.next(); cur && cur != head; cur=iter.next())
        indexes.push_back(*cur);

    std::vector<int>::iterator it = indexes.begin();
    std::vector<int>::iterator end = indexes.end();
    // per ogni dest:
    for (; it!=end; it++)
    {
        j = *it;
        notSatisfied = true;
        int demand = problem.activities[j];
        // w -> dim|i-j|
        for (w=1; w<nCells && notSatisfied; w++)
        {
            if (j-w < 0 && j+w > nCells)
            {
                feasible = false;
                break;
            }     // ----------------------------- not feasible

            vector<int> customers;
            for(int cust=0; cust < nCustomerTypes; cust++)
                customers.push_back(cust);


            random_shuffle ( customers.begin(), customers.end() );

            //for (m=nCustomerTypes-1; m >= 0 && notSatisfied; m--)
            for(vector<int>::iterator cIt = customers.begin(); cIt != customers.end(); cIt++)
            {
                m = *cIt;

            //per non sforare
                if (demand < problem.n[m])
                    continue;

                for (t=0; t<nTimeSteps && notSatisfied; t++)
                {

                    if (!(j-w < 0) && demand > 0)
                    {
                        i = j-w;
                        if (demand>problem.usersCell[i][m][t]*problem.n[m])
                        {
                            // le richieste sono maggiori dei task che possono soddisfare quegli utenti
                            // quindi mandiamoli tutti

                            solution[i][j][m][t] += problem.usersCell[i][m][t];
                            problem.usersCell[i][m][t] -= solution[i][j][m][t];
                            demand -= solution[i][j][m][t]*problem.n[m];

                        }
                        else
                        {
                            // altrimenti mandiamo solo quelli necessari (demand/task)
                            int sent = demand / problem.n[m];
                            solution[i][j][m][t] += sent;
                            problem.usersCell[i][m][t] -= sent;
                            demand -= sent*problem.n[m];
                            //notSatisfied = false;
                            //continue;
                        }
                    }
                    if (!(j+w >= nCells) && demand > 0)
                    {
                        i = j+w;
                        if (demand>problem.usersCell[i][m][t]*problem.n[m])
                        {
                            // le richieste sono maggiori dei task che possono soddisfare quegli utenti
                            // quindi mandiamoli tutti

                            solution[i][j][m][t] += problem.usersCell[i][m][t];
                            problem.usersCell[i][m][t] -= solution[i][j][m][t];
                            demand -= solution[i][j][m][t]*problem.n[m];
                        }
                        else
                        {
                            // altrimenti mandiamo solo quelli necessari (demand/task)
                            int sent = demand / problem.n[m];
                            solution[i][j][m][t] += sent;
                            problem.usersCell[i][m][t] -= sent;
                            demand -= sent*problem.n[m];
                            //notSatisfied = false;
                            //continue;
                        }
                    }

                    // controllo se ho soddisfatto le richieste
                    if(demand == 0)
                        notSatisfied = false;
                }
            }
        }
    }

    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    objfun += solution[j][i][m][t] * problem.costs[j][i][m][t];

    ////////////////////////////////////////////////////////////////////////////////////////////7
    if (!feasible){
        objfun = 10000000;
    }




    return (float) floor(objfun);


}

void Initializer(GAGenome& g)
{

    GAListGenome<int> &child=(GAListGenome<int> &)g;
//    while(child.head()) child.destroy(); // destroy any pre-existing list

    int listLength = _heuristic.getCells();
    Data instance = _heuristic.getProblem();
    int* tasks = instance.activities;
    for(int i=0; i<listLength; i++){
        if(tasks[i] > 0)
            child.insert(i);
    }

    int n = child.size();


    for(int j=0; j<n; j++)
        child.swap(GARandomInt(0,n-1), GARandomInt(0,n-1));
}


template<> int
GAListGenome<int>::write(ostream & os) const
{
    int *cur, *head;
    GAListIter<int> tmpiter(*this);
    if((head=tmpiter.head()) != 0) os << *head << " ";
    for(cur=tmpiter.next(); cur && cur != head; cur=tmpiter.next())
        os << *cur << " ";
    return os.fail() ? 1 : 0;
}
