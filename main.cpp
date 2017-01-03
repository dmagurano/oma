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
#include "window.h"


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

        int popsize  = 5;
        int ngen     = 1000000;
        float pmut   = 0.01;
        float pcross = 0.9;


        GAListGenome<int> genome(Objective);
        genome.initializer(Initializer);
        genome.mutator(GAListGenome<int>::SwapMutator);
        genome.crossover(GAListGenome<int>::PartialMatchCrossover);


        GASimpleGA ga(genome);// create the GA


        ga.populationSize(popsize);
        ga.nGenerations(ngen);
        ga.pMutation(pmut);
        ga.pCrossover(pcross);

//        GASteadyStateGA ga(genome);
//
//        GASigmaTruncationScaling trunc;
//        ga.scaling(trunc);
//        ga.set(gaNpopulationSize, 1000);
//        ga.set(gaNpCrossover, 0.6);
//        ga.set(gaNpMutation, 0.1);
//        ga.set(gaNnGenerations, 10000);
//        ga.set(gaNpReplacement, 0.1);


        ga.minimize();



        ga.initialize();

        clock_t tStart = clock();

        while( ((double)(clock() - tStart) / CLOCKS_PER_SEC ) < 1.0 ){
            ga.step();
        }

        stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

		cout << "best individual is " << ga.statistics().bestIndividual() << "\n\n";
		cout << ga.statistics() << "\n";

        for(int i=0; i<ga.statistics().bestPopulation().size(); i++){
                genome = ga.statistics().bestPopulation().individual(i);
                cout << genome << "\n";
        }

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

        //_heuristic.solveGreedy3(stat,order,problem);
        _heuristic.solveGreedy4(stat,order,_heuristic.getProblem());

        // lavoriamo con il risultato della diofantina per partire in una tabu search

       //_heuristic.solveDio(stat,order,_heuristic.getProblem());

       // _heuristic.solveTabu(stat, order, problem);






        _heuristic.hasSolution = true;

        /////////////////////////////////////////////////////////////////

		_heuristic.getStatSolution(stat);
		// Write KPI of solution
        string instanceName = splitpath(_inPath);

        cout << "Instance name: " << instanceName << endl;

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

float Objective_greedyrand(GAGenome& g){

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

                    bool random = GAFlipCoin(0.5);

                    if (!(j-w < 0) && demand > 0 && !random)
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
                    if (!(j+w >= nCells) && demand > 0 && random)
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

float Objective_dio(GAGenome& g){

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
    for (; it!=end; it++)
    {
        j = *it;
        notSatisfied = true;
        int demand = problem.activities[j];

        again: ;
        // w -> dim|i-j|
        for (w=1; w<nCells && notSatisfied; w++)
        {
            if (j-w < 0 && j+w >= nCells)
            {
                //feasible = false;
                break;
            }     // ----------------------------- not feasible

            // select a source nearby
            int i;
            for (int c = 0; c < 2 && demand > 0; c++)
            {


                if (c == 0)
                    i = j + w;
                else if ( c == 1)
                    i = j - w;
                //per evitare di sforare l'index
                if (j+w >= nCells)
                    i = j - w;
                if (j-w < 0)
                    i = j + w;

                float objfunct = 0;
                float min = INT64_MAX;
                int min_x, min_y, min_z, min_t;
                diophantine_solver ds = diophantine_solver(problem.n[0], problem.n[1], problem.n[2], demand);
//                int m = 1000, k = 1000;
                int m=rand()%10,k=rand()%10;
                bool foundSol = false;
                for (int t = 0; t < nTimeSteps; t++)
                {
                    if (problem.usersCell[i][0][t] == 0 && problem.usersCell[i][1][t] == 0 && problem.usersCell[i][2][t] == 0)
                        continue;
                    objfunct = 0;
                    ds.setTentativi(100);
                    float w1, w2, w3;
                    w3 = problem.costs[i][j][0][t] + problem.costs[i][j][1][t] + problem.costs[i][j][2][t];
                    w1 = problem.costs[i][j][0][t] / w3;
                    w2 = problem.costs[i][j][1][t] / w3;
                    w3 = problem.costs[i][j][2][t] / w3;
                    ds.setWeightX(w1);
                    ds.setWeightY(w2);
                    ds.setWeightZ(w3);
                    foundSol = ds.solve(&m, &k, problem.usersCell[i][0][t], problem.usersCell[i][1][t],
                                        problem.usersCell[i][2][t]).size() > 0;

                    // implementare una strategia per scegliere quanti utenti recuperare
                    if (ds.isSolved()) {
                        // se è possibile distribuire gli utenti valuta i costi della soluzione su tutti gli istanti temporali
                        objfunct = ds.getX() * problem.costs[i][j][0][t] +
                                   ds.getY() * problem.costs[i][j][1][t] +
                                   ds.getZ() * problem.costs[i][j][2][t];
                        if (objfunct < min) {
                            min = objfunct;
                            min_x = ds.getX();
                            min_y = ds.getY();
                            min_z = ds.getZ();
                            min_t = t;
                        }

                    }
                }
                if(ds.isSolved()){
                    solution[i][j][0][min_t] = min_x;
                    solution[i][j][1][min_t] = min_y;
                    solution[i][j][2][min_t] = min_z;

                    for (int m = 0; m < nCustomerTypes; m++) {
                        problem.usersCell[i][m][min_t] -= solution[i][j][m][min_t];
                        demand -= solution[i][j][m][min_t] * problem.n[m];
                        //for debug

                    }
                    problem.activities[j] = demand;
                }
                //check if demand has been satisfied

                if(demand <= 0){
                    notSatisfied = false;
                    break;
                }



            }




        }// for window

        if (demand > 0 && demand < 200) {
            demand++;
            goto again;
        }

        if (demand >= 200){
            feasible = false;
        }


        //next j

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

    bool notSolved = true;
    clock_t tStart = clock();

    float p;
    if(nCells > 100)
        p=0.15;
    else
        p=0.5;



    while(notSolved && (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < 5.0 )) {

        std::random_shuffle(indexes.begin(), indexes.end());
        std::vector<int>::iterator it = indexes.begin();
        std::vector<int>::iterator end = indexes.end();
//        std::vector<int>::iterator it3 ;
//        for(it3=indexes.begin(); it3!= indexes.end();it3++){
//            cout << " " << *it3;
//        }
//        cout << endl;

        for (; it != end; it++) {
            j = *it;
            //notSatisfied = true;
            int demand = problem.activities[j];
            if(demand <= 0)
                continue;
            // w -> dim|i-j|
            int minCost = 100000;
            int min_i = 0;
            int min_m = 0;
            int min_t = 0;
            for (w = 1; w < nCells; w++) {
//                if (j - w < 0 && j + w >= nCells) {
//                    //feasible = false;
//                    break;
//                }     // ----------------------------- not feasible



                //vedo qual'è il costo minore nella finestra w
                vector<int> customers;
                for (int cust = 0; cust < nCustomerTypes; cust++)
                    customers.push_back(cust);


                random_shuffle(customers.begin(), customers.end());
//                for (m=0; m < nCustomerTypes; m++){
                for (vector<int>::iterator cIt = customers.begin(); cIt != customers.end(); cIt++) {

                    m = *cIt;
                    //per non sforare con i task
                    //                if (demand < problem.n[m])
                    //                    continue;

                    for (t = 0; t < nTimeSteps; t++) {
                        if (!(j - w < 0)) {
                            i = j - w;
                            if ((problem.costs[i][j][m][t] / problem.n[m]) <= minCost &&
                                problem.usersCell[i][m][t] > 0 && problem.n[m] <= demand) {
                                if (problem.costs[i][j][m][t] == minCost) {
                                    if ((((double) (rand() % 101)) / 100) <= 1) {
                                        minCost = (problem.costs[i][j][m][t] / problem.n[m]);
                                        min_i = i;
                                        min_m = m;
                                        min_t = t;
                                    }
                                } else {
                                    minCost = (problem.costs[i][j][m][t] / problem.n[m]);
                                    min_i = i;
                                    min_m = m;
                                    min_t = t;
                                }

                            }
                        }
                        if (!(j + w >= nCells)) {
                            i = j + w;
                            if ((problem.costs[i][j][m][t] / problem.n[m]) <= minCost &&
                                problem.usersCell[i][m][t] > 0 && problem.n[m] <= demand) {
                                if (problem.costs[i][j][m][t] == minCost) {
                                    if ((((double) (rand() % 101)) / 100) <= 1) {
                                        minCost = (problem.costs[i][j][m][t] / problem.n[m]);
                                        min_i = i;
                                        min_m = m;
                                        min_t = t;
                                    }
                                } else {
                                    minCost = (problem.costs[i][j][m][t] / problem.n[m]);
                                    min_i = i;
                                    min_m = m;
                                    min_t = t;
                                }
                            }

                        }
                    }
                }


                //            int sent = demand / problem.n[min_m];
                //            if (sent == 0)
                //                sent++;
                //            if(sent > problem.usersCell[min_i][min_m][min_t]){
                //                sent = problem.usersCell[min_i][min_m][min_t];
                //            }

                //assegno solo un utente per volta poi ricomincio a ciclare su j

                if (problem.usersCell[min_i][min_m][min_t] == 0)
                    continue; // non ci sono utenti, allarga la finestra w



                if (w >= p * nCells) {
                    solution[min_i][j][min_m][min_t]++;
                    problem.usersCell[min_i][min_m][min_t]--;
                    demand -= problem.n[min_m];
                    problem.activities[j] = demand;
                    break;
                }




//                // controllo se ho soddisfatto le richieste
////                if (demand <= 0)
////                    notSatisfied = false;
//
//                problem.activities[j] = demand;
//
//                break; // ho assegnato un utente, passo alla prossima j
            }

//            solution[min_i][j][min_m][min_t]++;
//            problem.usersCell[min_i][min_m][min_t]--;
//            demand -= problem.n[min_m];
//            problem.activities[j] = demand;

        }

        bool again = false;
        //controllo se ho risolto il problema
        std::vector<int>::iterator it2 ;
        for(it2=indexes.begin(); it2!= indexes.end();it2++){
            if(problem.activities[*it2] > 0){
                again = true;
            }
        }

        if(!again) //tutte richieste soddisfatte
            notSolved = false;


    }//while

    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    objfun += solution[j][i][m][t] * problem.costs[j][i][m][t];

    ////////////////////////////////////////////////////////////////////////////////////////////7
    if (notSolved){
        objfun = 10000000;
    }




    return (float) floor(objfun);


}

float Objective_realgreedy1(GAGenome& g){

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
    for (; it!=end; it++)
    {
        j = *it;
        notSatisfied = true;
        int demand = problem.activities[j];
        // w -> dim|i-j|
        for (w=1; w<nCells && notSatisfied; w++)
        {
            if (j-w < 0 && j+w >= nCells)
            {
                feasible = false;
                break;
            }     // ----------------------------- not feasible

            int minCost = 100000;
            int min_i = 0;
            int min_m = 0;
            int min_t = 0;

            for (m=nCustomerTypes-1; m >= 0 && notSatisfied; m--)
            {
                //per non sforare con i task
//                if (demand < problem.n[m])
//                    continue;

                for (t=0; t<nTimeSteps && notSatisfied; t++)
                {
                    if (!(j-w < 0))
                    {
                        i = j-w;
                        if(problem.costs[i][j][m][t] < minCost && problem.usersCell[i][m][t] > 0){
                            minCost = problem.costs[i][j][m][t];
                            min_i = i;
                            min_m = m;
                            min_t = t;
                        }
                    }
                    if (!(j+w >= nCells))
                    {
                        i = j + w;
                        if(problem.costs[i][j][m][t] < minCost && problem.usersCell[i][m][t] > 0){
                            minCost = problem.costs[i][j][m][t];
                            min_i = i;
                            min_m = m;
                            min_t = t;
                        }

                    }
                }
            }


            int sent = demand / problem.n[min_m];
            if (sent == 0)
                sent++;
            if(sent > problem.usersCell[min_i][min_m][min_t]){
                sent = problem.usersCell[min_i][min_m][min_t];
            }
            solution[min_i][j][min_m][min_t] += sent;
            problem.usersCell[min_i][min_m][min_t] -= sent;
            demand -= sent*problem.n[min_m];



            // controllo se ho soddisfatto le richieste
            if(demand <= 0)
                notSatisfied = false;
            else //for debug
                problem.activities[j] = demand;
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
