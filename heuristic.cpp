#include <iostream>
#include <random>
#include <algorithm>
#include "heuristic.h"


using namespace std;

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
    std::replace(line.begin(), line.end(), ';', ' ');
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
    std::replace(line.begin(), line.end(), ';', ' ');
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
    std::replace(line.begin(), line.end(), ';', ' ');
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
            std::replace(line.begin(), line.end(), ';', ' ');
            istringstream issU(line);
            for (int i = 0; i < nCells; i++) {
                issU >> word;
                problem.usersCell[i][m][t] = atoi(word.c_str());
            }
        }
    }
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

float Heuristic::solveGreedy( vector<double>& stat, vector<int> indexes, Data problem) {

    clock_t tStart = clock();

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    int i, j, m, t, w;
    bool notSatisfied;
    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    solution[i][j][m][t] = 0;

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
            for (m=nCustomerTypes-1; m >= 0 && notSatisfied; m--)
            {
                //per non sforare con i task
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
                    else //for debug
                        problem.activities[j] = demand;
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
    if (!feasible) {
        cout << "Not feasible solution!" << "\n\n";

        cout << "Tasks to do: ";
        for(int i=0; i<nCells; i++)
           cout << problem.activities[i] << " ";
        cout << "\n\n";


        for(int m=0;m<nCustomerTypes;m++)
            for(int t=0;t<nTimeSteps;t++){
                cout << "User type: " << m << ", time: " << t << endl;
                for(int i=0; i<nCells; i++)
                    cout << problem.usersCell[i][m][t] << " ";
                cout << endl;
            }



    }
    stat.push_back(objfun);
    stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

    hasSolution=true;


    return (float) floor(objfun);
}

float Heuristic::solveDio( vector<double>& stat, vector<int> indexes, Data problem) {

    clock_t tStart = clock();

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    int i, j, m, t, w;
    bool notSatisfied;
    for (i = 0; i < nCells; i++)
        for (j = 0; j < nCells; j++)
            for (m = 0; m < nCustomerTypes; m++)
                for (t = 0; t < nTimeSteps; t++)
                    solution[i][j][m][t] = 0;

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
            if (j-w < 0 && j+w >= nCells)
            {
                feasible = false;
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
                int m=0,k=0;
                bool foundSol = false;
                for (int t = 0; t < nTimeSteps; t++)
                {
                    objfunct = 0;
                    ds.setTentativi(100);
                    ds.setWeightX(problem.costs[i][j][0][t]);
                    ds.setWeightY(problem.costs[i][j][1][t]);
                    ds.setWeightZ(problem.costs[i][j][2][t]);
                    foundSol = ds.solve(&m, &k, problem.usersCell[i][0][t], problem.usersCell[i][1][t],
                                                problem.usersCell[i][2][t]).size() > 0;

                    // implementare una strategia per scegliere quanti utenti recuperare
                    if (foundSol) {
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

                    }//if
                }
                if(foundSol){
                    solution[i][j][0][min_t] = min_x;
                    solution[i][j][1][min_t] = min_y;
                    solution[i][j][2][min_t] = min_z;

                    for (int m = 0; m < nCustomerTypes; m++) {
                        problem.usersCell[i][m][min_t] -= solution[i][j][m][min_t];
                        demand -= solution[i][j][m][min_t] * problem.n[m];
                        //for debug
                        problem.activities[j] = demand;
                    }
                }
                //check if demand has been satisfied

                if(demand <= 0){
                    notSatisfied = false;
                    break;
                }


            }




        }
    }


    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    objfun += solution[j][i][m][t] * problem.costs[j][i][m][t];

    ////////////////////////////////////////////////////////////////////////////////////////////7
    if (!feasible) {
        cout << "Not feasible solution!" << "\n\n";

        cout << "Tasks to do: ";
        for(int i=0; i<nCells; i++)
            cout << problem.activities[i] << " ";
        cout << "\n\n";


        for(int m=0;m<nCustomerTypes;m++)
            for(int t=0;t<nTimeSteps;t++){
                cout << "User type: " << m << ", time: " << t << endl;
                for(int i=0; i<nCells; i++)
                    cout << problem.usersCell[i][m][t] << " ";
                cout << endl;
            }



    }
    stat.push_back(objfun);
    stat.push_back((double)(clock() - tStart) / CLOCKS_PER_SEC);

    hasSolution=true;


    return (float) floor(objfun);
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