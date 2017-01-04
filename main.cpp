#include <list>
#include <iostream>

#include <list>
#include <random>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <chrono>
#include <thread>
#include <future>
#include <stdlib.h>
#include "utils.h"
#include "heuristic.h"
#define MAXTHREAD 2 // num of threads
#define TH_TIME 4.0 // threads loop limit
#define MAIN_TIME 4.0 // main thread waiting (for solutions) time limit
#define CHECK_RATE 1 // sleep interval main thread
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

int ****bestSolution;
double bestScore;
void thread_function(Heuristic& _heuristic);

std::mutex sol_m;

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

        bestScore = 10000000;
        struct timeval;

        vector<double> stats[MAXTHREAD];
        int th;
        clock_t start = clock();
        for (th=0; th<MAXTHREAD; th++)
        {
            std::thread t(thread_function, std::ref(_heuristic));
            t.detach();
        }
        std::cout << "Threads launched! Checking time...." << std::endl;
        // check solution
        double stop = 0;
        while (1)
        {
            stop =  (double)(clock() - start) / CLOCKS_PER_SEC;
            if (stop > MAIN_TIME)
                break;
            std::this_thread::sleep_for(std::chrono::seconds(CHECK_RATE));
        }
        std::unique_lock<std::mutex> l(sol_m);
        std::cout << "Main thread: get the final score." << std::endl;
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
        std::cout << "Processing completed." << std::endl << "Best score: " << bestScore << std::endl;
        vector<double> bestStat;
        bestStat.push_back(bestScore);
        bestStat.push_back(stop);
        _heuristic.replaceSolution(bestSolution);
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

	return 0;
}

void thread_function(Heuristic& _heuristic) {
    std::cout << "New thread started." << std::endl;
    int listLength = _heuristic.getCells();
    Data instance = _heuristic.getProblem();
    int *tasks = instance.activities;
    int nCells, nCustomerTypes, nTimeSteps;
    nCells = _heuristic.getCells();
    nCustomerTypes = _heuristic.getCustomers();
    nTimeSteps = _heuristic.getTimeSteps();
    //vector<double> stat;
    clock_t tStart = clock();
    // allocate solution
    int ****solution;
    solution = new int***[nCells];
    for (int i = 0; i < nCells; i++) {
        solution[i] = new int**[nCells];
        for (int j = 0; j < nCells; j++) {
            solution[i][j] = new int*[nCustomerTypes];
            for (int m = 0; m < nCustomerTypes; m++) {
                solution[i][j][m] = new int[nTimeSteps];
            }
        }
    }
    while (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < TH_TIME)
    {
        //stat.clear();
        vector<int> order;
        for (int i = 0; i < listLength; i++) {
            if (tasks[i] > 0)
                order.push_back(i);
        }
        srand(hash3(time(0), chrono::high_resolution_clock::now().time_since_epoch().count(), 1));
        random_shuffle(order.begin(), order.end(), myrand);
        // ###########################################################################################
        // ########### SOLVE ############
        float objfun = _heuristic.solveWinner(order, solution);
        std::cout << "New score " << objfun << std::endl;
        // ##########################################################################################

        //float objfun = 0;
        /*
        for (int i = 0; i < nCells; i++)
            for (int j = 0; j < nCells; j++)
                for (int m = 0; m < nCustomerTypes; m++)
                    for (int t = 0; t < nTimeSteps; t++)
                        objfun += solution[j][i][m][t] * instance.costs[j][i][m][t];
        */
        if (objfun < bestScore)
        {
            std::unique_lock<std::mutex> l(sol_m);
            bestScore = objfun;
            for (int i = 0; i < nCells; i++)
                for (int j = 0; j < nCells; j++)
                    for (int m = 0; m < nCustomerTypes; m++)
                        for (int t = 0; t < nTimeSteps; t++)
                            bestSolution[j][i][m][t] = solution[j][i][m][t];
            /*bestStat.clear();
            bestStat = stat;*/
        }



    }

}

