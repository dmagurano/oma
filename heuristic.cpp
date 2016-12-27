#include <iostream>
#include <random>
#include <algorithm>
#include "heuristic.h"
#include "group.h"
#include "tabu_list.h"
#include "simulated_annealing.h"


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

float Heuristic::solveTabu(vector<double> &stat, vector<int> indexes, Data problem)
{
    tabu_list ts = tabu_list(rand()%20);
    float objfun = 0;
    for (int i = 0; i < nCells; i++)
        for (int j = 0; j < nCells; j++)
            for (int m = 0; m < nCustomerTypes; m++)
                for (int t = 0; t < nTimeSteps; t++)
                    objfun += solution[j][i][m][t] * problem.costs[j][i][m][t];

    // temperatura iniziale posta a 12400 in modo da avere p0 = 0.5
    simulated_annealing sa = simulated_annealing(0.4, 12400, rand() % (20 - 10) + 10, objfun);

    std::vector<int>::iterator it = indexes.begin();
    std::vector<int>::iterator end = indexes.end();
    int j;
    for (; it!=end; it++)
    {
        j = *it; //j origin cell
        if (problem.activities[j] == 0)
        {
            continue;
        }
        int group1_type, group2_type;
        int mcm;
        bool group1_present, group2_present;
        int group1_source_index, group2_source_index;
        int group1_time, group2_time;


        do {
            group1_type = rand() % 3;
            group1_present = false;
            for (int i = 0; i < nCells && !group1_present; i++) {
                if (i != j) {
                    for (int t = 0; t < nTimeSteps && !group1_present; t++) {
                        if (solution[i][j][group1_type][t] > 0) {
                            group1_present = true;
                            group1_source_index = i;
                            group1_time = t;
                            break;
                        }
                    }
                }
            }
        } while (group1_present == false);

        int destination2;

        do
        {
            destination2 = rand() % nCells;
            if (destination2 == j || problem.activities[destination2] == 0)
            {
                continue;
            }
            group2_type = rand() % 3;
            group2_present = false;
            for (int i = 0; i < nCells && !group2_present; i++) {
                if (i != j) {
                    for (int t = 0; t < nTimeSteps && !group2_present; t++) {
                        if (solution[i][destination2][group2_type][t] > 0) {
                            group2_present = true;
                            group2_source_index = i;
                            group2_time = t;
                            break;
                        }
                    }
                }
            }
        } while (group2_present == false);


        mcm = diophantine_solver::mcm(problem.n[group1_type], problem.n[group2_type]);

        group group1 = group(mcm, group1_source_index, j, group1_type,problem.n[group1_type], group1_time, solution[group1_source_index][j][group1_type][group1_time]);

        group group2 = group(group1, mcm, group2_source_index, destination2, group2_type, problem.n[group2_type],group2_time, solution[group2_source_index][destination2][group2_type][group2_time]);

        if (group1.getGroup_capability() != group2.getGroup_capability())
            // dovremmo pensare di rivisitare la cella
            continue;
        // calcolo della nuova objective function
        float new_obj_funct = objfun + group1.cost(group2.getDestination_cell(), group2.getTime_step(), problem.costs) + group2.cost(group1.getDestination_cell(), group1.getTime_step(), problem.costs);

        //inibita momentaneamente la tabu list visto che non funziona
        if (sa.accept_solution(new_obj_funct) && ! ts.check_move(group1, group2))
        //if (sa.accept_solution(new_obj_funct))
        {
            ts.add_move(&group1, &group2);
            sa.setCurrent_objective_function(new_obj_funct);
            objfun = new_obj_funct;
            // remove users from the cells
            solution[group1.getSource_cell()][group1.getDestination_cell()][group1.getType_of_people()][group1.getTime_step()] -= group1.getXj();
            solution[group2.getSource_cell()][group2.getDestination_cell()][group2.getType_of_people()][group2.getTime_step()] -= group2.getXj();
            // add users in the cells
            solution[group1.getSource_cell()][group2.getDestination_cell()][group1.getType_of_people()][group2.getTime_step()] += group1.getXj();
            solution[group2.getSource_cell()][group1.getDestination_cell()][group2.getType_of_people()][group1.getTime_step()] += group2.getXj();
        }


    }

        /*int min_task[3], max_task[3];

        int group_activities_origin;
        int group_activities_destination;

        for (int i = 0; i < nCells; i++)
        {
            for (int m = 0; m < nCustomerTypes; m++)
            {
                for (int t = 0; t < nTimeSteps; t++)
                {
                    if (i != j)
                    {
                        if (solution[i][j][m][t] > 0)
                        {
                            if (min_task[m] < problem.n[m])
                            {
                                min_task[m] = problem.n[m];
                            }
                            if (max_task[m] > problem.n[m])
                            {
                                max_task[m] = problem.n[m] * solution[i][j][m][t];
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 3; i++)
        {
            group_activities_origin += rand() % (max_task[i] - min_task[i]) + min_task[i];
        }

        int destination_cell;
        do
        {
            destination_cell = rand() % nCells;
        } while (destination_cell == j || problem.activities[destination_cell] == 0);

        for (int i = 0; i < nCells; i++)
        {
            for (int m = 0; m < nCustomerTypes; m++)
            {
                for (int t = 0; t < nTimeSteps; t++)
                {
                    if (i != j)
                    {
                        if (solution[i][j][m][t] > 0)
                        {
                            if (min_task[m] < problem.n[m])
                            {
                                min_task[m] = problem.n[m];
                            }
                            if (max_task[m] > problem.n[m])
                            {
                                max_task[m] = problem.n[m] * solution[i][j][m][t];
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 3; i++)
        {
            group_activities_destination += rand() % (max_task[i] - min_task[i]) + min_task[i];
        }
    }*/


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

                    }//if
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
                    //problem.activities[j] = demand;
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