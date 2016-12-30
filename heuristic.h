#ifndef COIOTE_HEURISTIC_HEURISTIC_H
#define COIOTE_HEURISTIC_HEURISTIC_H

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <list>
#include <map>
#include <cstring>
#include "diophantine_solver.h"

using namespace std;

struct Data {
    /**
     * The costs ijkt
     */
    double**** costs;

    /**
     * number of activity done by one user of type k
     */
    int* n;

    /**
     * Activities to be done in node i during the given time horizon
     */
    int* activities;

    /**
     * Number of users of type m in i during time period t (ikt)
     */
    int*** usersCell;
};

enum eFeasibleState {
    FEASIBLE,
    NOT_FEASIBLE_DEMAND,
    NOT_FEASIBLE_USERS
};

class Heuristic{
public:
    /**
     * Number of periods
     */
    int nTimeSteps;

    /**
     * Number of customer types
     */
    int nCustomerTypes;

    /**
     * Number of cells
     */
    int nCells;

    /**
     * Problem structure for parameters
     */
    Data problem;

    /**
     * Flag equals to true if the problems has a solution
     */
    bool hasSolution;

    /**
     * Variables of the problem (X in the model)
     * first index is source cell second index is destination third is type fourth is time
     */
    int**** solution;

public:
    /**
     * Default constructor
     * @return Heuristic object
     */
    Heuristic(){};

    /**
     * Constructor from external file
     * @param path path of the external file cotaining the instance of the problem
     * @return
     */
    Heuristic(string path);

    /**
     * Function to CHANGE!!! This function only makes a very bad solution for the problem
     * @param stat Array of statistics. In position 0 put the objVal, in position 1 the computational time
     * @param timeLimit Time limit for computation
     * @param verbose
     */
    void solveFast(vector<double>& stat, int timeLimit = - 1);

    /**
     * Puts KPIs in the statistics' array. Call this only if problem has a solution
     * @param stat Array of statistics
     */
    void getStatSolution(vector<double>& stat);

    /**
     * Write KPIs on a file
     * @param path path of the file
     * @param nameInstance name of the instance
     * @param stat array of statistics
     */
    void writeKPI(string path, string nameInstance, vector<double> stat);

    /**
     * Write the detailed solution on a file
     * @param path path of the file
     */
    void writeSolution(string path);

    float solveGreedy(vector<double>& stat, vector<int> indexes, Data problem);
    float solveGreedy2(vector<double>& stat, vector<int> indexes, Data problem);
    float solveGreedy3(vector<double>& stat, vector<int> indexes, Data problem);
    float solveGreedy4(vector<double>& stat, vector<int> indexes, Data problem);
    float solveDio( vector<double>& stat, vector<int> indexes, Data problem);
    float solveTabu( vector<double>& stat, vector<int> indexes, Data problem);
    /**
     * Check the feasibility of the problem
     * @param path path of the solution file
     * @return a state of the check (i.e. FEASIBLE if the solution is feasible)
     */
    eFeasibleState isFeasible(string path);

    int getCells(){
        return nCells;
    };

    Data getProblem(){
        Data p;
        p.costs = problem.costs;
        p.n = problem.n;
        p.activities = new int[nCells];
        memcpy(p.activities, problem.activities, nCells*sizeof(int));
        p.usersCell = new int**[nCells];
        for (int i = 0; i < this->nCells; i++) {
            p.usersCell[i] = new int*[nCustomerTypes];
            //memcpy(p.usersCell[i], problem.usersCell[i], nCustomerTypes*sizeof(int));
            for (int m = 0; m < this->nCustomerTypes; m++) {
                p.usersCell[i][m] = new int[nTimeSteps];
                for (int t = 0; t<nTimeSteps; t++)
                    memcpy(&p.usersCell[i][m][t], &problem.usersCell[i][m][t], sizeof(int));

            }


        }

        return p;
    }

};

#endif //COIOTE_HEURISTIC_HEURISTIC_H