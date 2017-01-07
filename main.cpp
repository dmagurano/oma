#include <list>
#include <iostream>

#include <random>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <chrono>
#include <thread>
#include <future>

#include "utils.h"
#include "heuristic.h"
#define MAXTHREAD 8 // num of threads
#define TH_TIME 4.0 // threads loop limit
#define MAIN_TIME 4 // main thread waiting (for solutions) time limit
//#define CHECK_RATE 1 // sleep interval main thread
using namespace std;


unsigned int hash3(unsigned int h1, unsigned int h2, unsigned int h3)
{
    unsigned int a = ((h1*2654435789U)+h2)*2654435789U + h3;
    return a;
}

int myrand(int i) {
    //srand(unsigned(time(0)));
    return rand()%i;
}

struct thread_result
{
    std::thread *t;
    float obj_fun;
    float reduction_factor;
    vector<int> order;
    int**** solution;
    bool valid;
};

int ****bestSolution;
double bestScore;
void thread_function(Heuristic& _heuristic, thread_result* tr);


//std::mutex sol_m;

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
        //vector<double> stat;
        //_heuristic.solveFast(stat);
        int nCells, nCustomerTypes, nTimeSteps;
        nCells = _heuristic.getCells();
        nCustomerTypes = _heuristic.getCustomers();
        nTimeSteps = _heuristic.getTimeSteps();
        // allocate BEST solution

        bestSolution = new int***[nCells];
        for (int i = 0; i < nCells; i++) {
            bestSolution[i] = new int**[nCells];
            for (int j = 0; j < nCells; j++) {
                bestSolution[i][j] = new int*[nCustomerTypes];
                for (int m = 0; m < nCustomerTypes; m++) {
                    bestSolution[i][j][m] = new int[nTimeSteps];
                }
            }
        }
        srand(time(NULL));
        bestScore = 10000000;
        struct timeval;

        //vector<double> stats[MAXTHREAD];
        thread_result *trs = new thread_result[MAXTHREAD];
        int th;

        clock_t start = clock();
        for (th=0; th<MAXTHREAD; th++)
        {
            if (nCells > 100) {
                if (th == 0)
                    trs[th].reduction_factor = 0.5;
                else if (th == 1)
                    trs[th].reduction_factor = 0.15;
                else
                    trs[th].reduction_factor = (float) ((rand() % 80 - 15) + 15) / 100;
            }
            else
            {
                if (th == 0)
                    trs[th].reduction_factor = 1;
                else if (th == 1)
                    trs[th].reduction_factor = 0.25;
                else
                    trs[th].reduction_factor = (float) ((rand() % 101 - 25) + 25) / 100;
            }
            trs[th].obj_fun = 10000000;
            trs[th].t = new std::thread(thread_function, std::ref(_heuristic), &trs[th]);

            trs[th].t->detach();

        }

        std::this_thread::sleep_for(std::chrono::seconds(MAIN_TIME));
        //std::cout << "Threads launched! Checking time...." << std::endl;
        // check solution
        /*double stop = 0;

        while (1)
        {
            stop =  (double)(clock() - start) / CLOCKS_PER_SEC;
            if (stop > MAIN_TIME)
                break;
            std::this_thread::sleep_for(std::chrono::seconds(CHECK_RATE));
        }
        //std::unique_lock<std::mutex> l(sol_m);
        //std::cout << "Main thread: get the final score." << std::endl;*/

        thread_result *mintr = &trs[0];
        double stop = (double) (clock() - start) / CLOCKS_PER_SEC;


        for (th = 1; th < MAXTHREAD; th++)
        {
            if (trs[th].valid && mintr->obj_fun > trs[th].obj_fun)
            {
                mintr = &trs[th];
            }
        }


        //
/*
        for (th=0; th<MAXTHREAD; th++)
        {
            if (results->wait_for(std::chrono::minutes(0)) == std::future_status::ready)
            {
                // thread end, evaluate its solution

            }
        }

*/
        if (mintr->valid)
        {
            _heuristic.replaceSolution(mintr->solution);
            bestScore = mintr->obj_fun;
        }
        std::cout << "Processing completed." << std::endl << "Best score: " << mintr->obj_fun << std::endl;
        vector<double> bestStat;
        bestStat.push_back(bestScore);
        bestStat.push_back(stop);

        if (bestScore < 10000000)
            _heuristic.setHasSolution(true);
        //_heuristic.getStatSolution(bestStat);
        // Write KPI of solution
        std::cout << "Printing the solution." << std::endl;
        string instanceName = splitpath(_inPath);
        _heuristic.writeKPI(_outPath, instanceName, bestStat);
        // Write solution
        if (!_solPath.empty())
            _heuristic.writeSolution(_solPath);
        std::cout << "End." << std::endl;

    }
    else {
        // Read the instance file
        Heuristic _heuristic(_inPath);
        // Read the solution file
        eFeasibleState _feasibility = _heuristic.isFeasible(_solPath);
        switch (_feasibility) {
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
    //std::this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}

void thread_function(Heuristic& _heuristic, thread_result* tr) {
    //std::cout << "New thread started." << std::endl;
    int listLength = _heuristic.getCells();
    Data instance = _heuristic.getProblem();
    int *tasks = instance.activities;
    int nCells, nCustomerTypes, nTimeSteps;
    nCells = _heuristic.getCells();
    nCustomerTypes = _heuristic.getCustomers();
    nTimeSteps = _heuristic.getTimeSteps();
    int seed;

    //vector<double> stat;
    clock_t tStart = clock();
    // allocate solution
    int ****solution;
    solution = new int***[nCells];
    tr->solution = new int***[nCells];
    for (int i = 0; i < nCells; i++)
    {
        solution[i] = new int**[nCells];
        tr->solution[i] = new int**[nCells];
        for (int j = 0; j < nCells; j++)
        {
            solution[i][j] = new int*[nCustomerTypes];
            tr->solution[i][j] = new int*[nCustomerTypes];
            for (int m = 0; m < nCustomerTypes; m++)
            {
                solution[i][j][m] = new int[nTimeSteps];
                tr->solution[i][j][m] = new int[nTimeSteps];

            }
        }
    }

    vector<int> order;
    for (int i = 0; i < listLength; i++) {
        if (tasks[i] > 0)
            order.push_back(i);
    }

    float objfun;
    while (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < TH_TIME)
    {

        //stat.clear();
        seed = hash3(time(0), chrono::high_resolution_clock::now().time_since_epoch().count(), 1);
        srand(seed);
        random_shuffle(order.begin(), order.end(), myrand);
        // ###########################################################################################
        // ########### SOLVE ############
        tr->valid = false;
        int i, j, m, t, w;
        bool notSatisfied;
        for (i = 0; i < nCells; i++)
            for (j = 0; j < nCells; j++)
                for (m = 0; m < nCustomerTypes; m++)
                    for (t = 0; t < nTimeSteps; t++)
                        solution[i][j][m][t] = 0;


        Data problem = _heuristic.getProblem();

        bool notSolved = true;
        clock_t tStart = clock();

        float p = tr->reduction_factor;
        if (p < 0) {
            if (nCells > 100)
                p = 0.25;
            else
                p = 0.5;
        }

        vector<int> customers;
        for (int cust = 0; cust < nCustomerTypes; cust++)
            customers.push_back(cust);

        //while(notSolved && (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < 5.0 )) {

        //std::random_shuffle(indexes.begin(), indexes.end());
        std::vector<int>::iterator it = order.begin();
        std::vector<int>::iterator end = order.end();

//        std::vector<int>::iterator it3 ;
//        for(it3=indexes.begin(); it3!= indexes.end();it3++){
//            cout << " " << *it3;
//        }
//        cout << endl;

        for (; it != end && ((double)(clock() - tStart) / CLOCKS_PER_SEC ) < TH_TIME; it++) {
            j = *it;
            //notSatisfied = true;
            int demand = problem.activities[j];
            while (demand > 0)
            {
                if (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < TH_TIME)
                {
                    tr->valid = false;
                }

                // w -> dim|i-j|
                int minCost = 100000;
                int min_i = 0;
                int min_m = 0;
                int min_t = 0;
                for (w = 1; w < nCells; w++)
                {
//                if (j - w < 0 && j + w >= nCells) {
//                    //feasible = false;
//                    break;
//                }     // ----------------------------- not feasible
                    random_shuffle(customers.begin(), customers.end());
//                for (m=0; m < nCustomerTypes; m++){
                    for (vector<int>::iterator cIt = customers.begin(); cIt != customers.end(); cIt++) {

                        m = *cIt;
                        //per non sforare con i task
                        //                if (demand < problem.n[m])
                        //                    continue;

                        for (t = 0; t < nTimeSteps; t++)
                        {

                            if (j - w >= 0)
                            {
                                i = j - w;
                                if ((problem.costs[i][j][m][t] / problem.n[m]) <= minCost &&
                                    problem.usersCell[i][m][t] > 0 && problem.n[m] <= demand) {
                                    if ((problem.costs[i][j][m][t] / problem.n[m]) == minCost) {
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
                            if (j + w < nCells)
                            {
                                i = j + w;
                                if ((problem.costs[i][j][m][t] / problem.n[m]) <= minCost &&
                                    problem.usersCell[i][m][t] > 0 && problem.n[m] <= demand) {
                                    if ((problem.costs[i][j][m][t] / problem.n[m]) == minCost) {
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
            }//while
        }

        bool again = false;
        //controllo se ho risolto il problema
        std::vector<int>::iterator it2 ;
        for(it2=order.begin(); it2!= order.end();it2++){
            if(problem.activities[*it2] > 0){
                again = true;
            }
        }

        if(!again) //tutte richieste soddisfatte
            notSolved = false;


        //}//while

        for (i = 0; i < nCells; i++)
            for (j = 0; j < nCells; j++)
                for (m = 0; m < nCustomerTypes; m++)
                    for (t = 0; t < nTimeSteps; t++) {
                        objfun += solution[j][i][m][t] * problem.costs[j][i][m][t];
                    }

        ////////////////////////////////////////////////////////////////////////////////////////////7
        if (notSolved){
            objfun = 10000000;
        }

        if (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < TH_TIME)
            tr->valid = true;
        //std::cout << "New score " << objfun << std::endl;
        // ##########################################################################################

        //float objfun = 0;
        /*
        for (int i = 0; i < nCells; i++)
            for (int j = 0; j < nCells; j++)
                for (int m = 0; m < nCustomerTypes; m++)
                    for (int t = 0; t < nTimeSteps; t++)
                        objfun += solution[j][i][m][t] * instance.costs[j][i][m][t];
        */
        if (objfun < tr->obj_fun)
        {
            tr->obj_fun = objfun;
            tr->order = order;
            tr->valid = false;
            for (int i = 0; i < nCells; i++)
                for (int j = 0; j < nCells; j++)
                    for (int m = 0; m < nCustomerTypes; m++)
                        for (int t = 0; t < nTimeSteps; t++)
                            tr->solution[j][i][m][t] = solution[j][i][m][t];
            tr->valid = true;
            /*std::unique_lock<std::mutex> l(sol_m);
            bestScore = objfun;
            for (int i = 0; i < nCells; i++)
                for (int j = 0; j < nCells; j++)
                    for (int m = 0; m < nCustomerTypes; m++)
                        for (int t = 0; t < nTimeSteps; t++)
                            bestSolution[j][i][m][t] = solution[j][i][m][t];*/
            /*bestStat.clear();
            bestStat = stat;*/
        }



    }
    tr->valid = true;
}

