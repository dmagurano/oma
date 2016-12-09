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
#include "utils.h"
#include "heuristic.h"

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



        //srand(unsigned(time(0)));
        struct timeval;
        //gettimeofday(&time, NULL);
        clock_t start = clock();
        while ((double) ((clock() - start) / CLOCKS_PER_SEC) < 60.0) {
            int listLength = _heuristic.getCells();
            Data instance = _heuristic.getProblem();
            int *tasks = instance.activities;
            vector<int> order;
            for (int i = 0; i < listLength; i++) {
                if (tasks[i] > 0)
                    order.push_back(i);
            }

            vector<double> stat;
            srand(hash3(time(0), chrono::high_resolution_clock::now().time_since_epoch().count(), getpid()));
            random_shuffle(order.begin(), order.end(), myrand);
            //auto engine = default_random_engine{};
            //std::shuffle(begin(order), end(order), engine);
            _heuristic.solveGreedy(stat, order, instance);


            /*
             * vector<int> test;
            test.push_back(17);
            test.push_back(14);
            test.push_back(27);
            test.push_back(18);
            test.push_back(10);
            test.push_back(11);
            test.push_back(13);
            test.push_back(1);
            test.push_back(29);
            test.push_back(0);
            test.push_back(28);
            test.push_back(19);
            test.push_back(4);
            test.push_back(16);
            test.push_back(9);

            _heuristic.solveGreedy(stat,test,instance);
            */

            _heuristic.getStatSolution(stat);
            // Write KPI of solution
            string instanceName = splitpath(_inPath);
            _heuristic.writeKPI(_outPath, instanceName, stat);
            // Write solution
            if (!_solPath.empty())
                _heuristic.writeSolution(_solPath);

           } //WHILE

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

