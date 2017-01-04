
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "heuristic.h"

using namespace std;

int myrand1(int i) {
    //srand(unsigned(time(0)));
    return rand()%i;
}

Heuristic::Heuristic(string path){
    this->hasSolution = false;
    string line;
    string word;

    ifstream iffN(path.c_str());

    if (!iffN.is_open()) {
        cout << "Impossible to open" << path << endl;
        cin.get();
        exit(1);
    }

    getline(iffN, line);
    replace(line.begin(), line.end(), ';', ' ');
    istringstream iss(line);
    iss >> word;
    this->nCells = atoi(word.c_str());
    iss >> word;
    this->nTimeSteps = atoi(word.c_str());
    iss >> word;
    this->nCustomerTypes = atoi(word.c_str());

    // Memory allocation
    solution = new int***[nCells];
    problem.costs = new double***[nCells];
    for (int i = 0; i < this->nCells; i++) {
        problem.costs[i] = new double**[nCells];
        solution[i] = new int**[nCells];
        for (int j = 0; j < this->nCells; j++) {
            problem.costs[i][j] = new double*[nCustomerTypes];
            solution[i][j] = new int*[nCustomerTypes];
            for (int m = 0; m < this->nCustomerTypes; m++) {
                problem.costs[i][j][m] = new double[nTimeSteps];
                solution[i][j][m] = new int[nTimeSteps];
            }
        }
    }
    problem.n = new int[nCustomerTypes];
    problem.activities = new int[nCells];
    problem.usersCell = new int**[nCells];
    for (int i = 0; i < this->nCells; i++) {
        problem.usersCell[i] = new int*[nCustomerTypes];
        for (int m = 0; m < this->nCustomerTypes; m++) {
            problem.usersCell[i][m] = new int[nTimeSteps];
        }
    }

    getline(iffN, line);
    getline(iffN, line);
    replace(line.begin(), line.end(), ';', ' ');
    istringstream issN(line);
    for (int m = 0; m < nCustomerTypes; m++) {
        issN >> word;
        problem.n[m] = atoi(word.c_str());
    }

    getline(iffN, line);
    for (int m = 0; m < nCustomerTypes; m++) {
        for (int t = 0; t < nTimeSteps; t++) {
            getline(iffN, line);// linea con m e t
            for (int i = 0; i < nCells; i++) {
                getline(iffN, line);// linea della matrice c_{ij} per t ed m fissati
                istringstream issC(line);
                for (int j = 0; j < nCells; j++) {
                    issC >> word;
                    problem.costs[i][j][m][t] = atoi(word.c_str());
                }
            }
        }
    }

    getline(iffN, line);
    getline(iffN, line);
    replace(line.begin(), line.end(), ';', ' ');
    istringstream issA(line);
    for (int i = 0; i < nCells; i++) {
        issA >> word;
        problem.activities[i] = atoi(word.c_str());
    }

    getline(iffN, line);
    for (int m = 0; m < nCustomerTypes; m++) {
        for (int t = 0; t < nTimeSteps; t++) {
            getline(iffN, line);
            getline(iffN, line);
            replace(line.begin(), line.end(), ';', ' ');
            istringstream issU(line);
            for (int i = 0; i < nCells; i++) {
                issU >> word;
                problem.usersCell[i][m][t] = atoi(word.c_str());
            }
        }
    }
}

float Heuristic::solveGreedy(vector<double>& stat, vector<int> indexes, Data problem) {

    clock_t tStart = clock();

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    int i, j, m, t, w;
    bool notSatisfied;
    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    solution[i][j][m][t] = 0;
    auto a = chrono::high_resolution_clock::now().time_since_epoch();
    auto now_ms = std::chrono::duration_cast<std::chrono::microseconds>(a);
    srand(hash3(now_ms.count(), getpid()));
    int  objfun = 0;
    bool feasible = true;
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

            //srand(hash3(time(0),chrono::high_resolution_clock::now().time_since_epoch().count(), getpid()));

            random_shuffle(customers.begin(), customers.end(), myrand1);
            for(int cust=0; cust < nCustomerTypes; cust++)
                cout << customers[cust];
            cout << endl;
            //for (m=nCustomerTypes-1; m >= 0 && notSatisfied; m--)
            for(vector<int>::iterator cIt = customers.begin(); cIt != customers.end(); cIt++)
            {

                m = *cIt;
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
    it = indexes.begin();
    for ( ; it != end; it++)
        cout << " " << *it;

    cout << endl;
    if (!feasible)
        cout << "Not feasible solution!" << endl;
    else
    {
        cout << "Feasible solution!" << endl;

    }
    cout << "------------------" << endl;


    stat.push_back(objfun);
    stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

    hasSolution=true;


    return (float) floor(objfun);
}

void Heuristic::solveFast(vector<double>& stat, int timeLimit) {
    double objFun=0;
    clock_t tStart = clock();

    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    solution[i][j][m][t] = 0;

    for (int i = 0; i < nCells; i++) {
        int demand = problem.activities[i];

        bool notSatisfied = true;
        for (int j = 0; j < nCells && notSatisfied; j++) {
            for (int m = 0; m < nCustomerTypes && notSatisfied; m++) {
                for (int t = 0; t < nTimeSteps && notSatisfied; t++) {
                    if (i != j) {
                        if (demand > problem.n[m] * problem.usersCell[j][m][t]) {
                            solution[j][i][m][t] = problem.usersCell[j][m][t];
                            problem.usersCell[j][m][t] -= solution[j][i][m][t];
                        }
                        else {
                            solution[j][i][m][t] += floor(demand / problem.n[m]);
                            notSatisfied = false;
                        }
                        if (solution[j][i][m][t] != 0)
                            objFun += solution[j][i][m][t] * problem.costs[j][i][m][t];
                        demand -= problem.n[m]*solution[j][i][m][t];
                    }
                }
            }
        }
    }


    stat.push_back(objFun);
    stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

    hasSolution=true;
}

void Heuristic::writeKPI(string path, string instanceName, vector<double> stat){
    if (!hasSolution)
        return;

    ofstream fileO(path, ios::app);
    if(!fileO.is_open())
        return;

    fileO << instanceName << ";" << stat[0] << ";" << stat[1];
    for(int i=2; i<stat.size(); i++)
        fileO <<  ";" << stat[i];
    fileO << endl;

    fileO.close();

}

void Heuristic::writeSolution(string path) {
    if (!hasSolution)
        return;

    ofstream fileO(path);
    if(!fileO.is_open())
        return;

    fileO << this->nCells << "; " << this->nTimeSteps << "; " << this->nCustomerTypes << endl;
    for (int m = 0; m < this->nCustomerTypes; m++)
        for (int t = 0; t < this->nTimeSteps; t++)
            for (int i = 0; i < this->nCells; i++)
                for (int j = 0; j < this->nCells; j++)
                    if (solution[i][j][m][t] > 0)
                        fileO << i << ";" << j << ";" << m << ";" << t << ";" << solution[i][j][m][t] << endl;

    fileO.close();
}

eFeasibleState Heuristic::isFeasible(string path) {

    string line;
    string word;
    int nCellsN;
    int nTimeStepsN;
    int nCustomerTypesN;
    int i, j, m, t;


    ifstream iffN(path.c_str());

    if (!iffN.is_open()) {
        cout << "Impossible to open" << path << endl;
        exit(1);
    }

    getline(iffN, line);
    std::replace(line.begin(), line.end(), ';', ' ');
    istringstream iss(line);
    iss >> word; // nCells
    nCellsN = atoi(word.c_str());
    iss >> word; // nTimeSteps
    nTimeStepsN = atoi(word.c_str());
    iss >> word; // nCustomerTypes
    nCustomerTypesN = atoi(word.c_str());

    int**** solutionN = new int***[nCells];
    for (i = 0; i < nCellsN; i++) {
        solutionN[i] = new int**[nCells];
        for (j = 0; j < nCellsN; j++) {
            solutionN[i][j] = new int*[nCustomerTypes];
            for (m = 0; m < nCustomerTypesN; m++) {
                solutionN[i][j][m] = new int[nTimeSteps];
                for ( t = 0; t < nTimeStepsN; t++) {
                    solutionN[i][j][m][t] = 0;
                }
            }
        }
    }

    while (getline(iffN, line)) {
        std::replace(line.begin(), line.end(), ';', ' ');
        istringstream iss(line);
        iss >> word; // i
        i = atoi(word.c_str());
        iss >> word; // j
        j = atoi(word.c_str());
        iss >> word; // m
        m = atoi(word.c_str());
        iss >> word; // t
        t = atoi(word.c_str());
        iss >> word; // value
        solutionN[i][j][m][t] = atoi(word.c_str());
    }

    // Demand
    bool feasible = true;
    int expr;
    for (int i = 0; i < nCells; i++) {
        expr = 0;
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    expr += problem.n[m] * solutionN[j][i][m][t];
        if (expr < problem.activities[i])
            feasible = false;
    }

    if (!feasible)
        return NOT_FEASIBLE_DEMAND;

    // Max Number of users
    for (int i = 0; i < nCells; i++)
        for (int m = 0; m < nCustomerTypes; m++)
            for (int t = 0; t < nTimeSteps; t++) {
                expr = 0;
                for (int j = 0; j < nCells; j++)
                    expr += solutionN[i][j][m][t];
                if (expr > problem.usersCell[i][m][t])
                    feasible = false;
            }

    if(!feasible)
        return NOT_FEASIBLE_USERS;

    return FEASIBLE;
}

void Heuristic::getStatSolution(vector<double>& stat) {
    if (!hasSolution)
        return;

    int* tipi = new int[nCustomerTypes];
    for (int m = 0; m < nCustomerTypes; m++)
        tipi[m] = 0;

    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int t = 0; t < nTimeSteps; t++)
                for (int m = 0; m < nCustomerTypes; m++)
                    if (solution[i][j][m][t] > 0)
                        tipi[m] += solution[i][j][m][t];
    for (int m = 0; m < nCustomerTypes; m++)
        stat.push_back(tipi[m]);

}

void Heuristic::replaceSolution(int ****newS) {
    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    this->solution[j][i][m][t] = newS[j][i][m][t];
}

float Heuristic::solveWinner(vector<int>& indexes, int ****solution)
{

    int i, j, m, t, w;
    bool notSatisfied;
    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    solution[i][j][m][t] = 0;

    int  objfun = 0;
    Data problem = getProblem();

    bool notSolved = true;
    clock_t tStart = clock();

    float p;
    if(nCells > 100)
        p=0.15;
    else
        p=0.25;

    while(notSolved && (((double)(clock() - tStart) / CLOCKS_PER_SEC ) < 5.0 )) {

        //std::random_shuffle(indexes.begin(), indexes.end());
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
            while (demand > 0){

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
                            if (!(j + w >= nCells)) {
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