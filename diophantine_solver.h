//
// Created by Francesco on 12/12/2016.
//
#include <list>
#include <stdlib.h>
#include <time.h>
#include <vector>

#ifndef COIOTE_HEURISTIC_DIOPHANTINE_SOLVER_H
#define COIOTE_HEURISTIC_DIOPHANTINE_SOLVER_H


class diophantine_solver
{

private:
    int a, b, c, d;
    int x, y, z;

    public:
        diophantine_solver(int a, int b, int c, int d);

        std::vector<int> solve(int *m, int *k, int max_x, int max_y, int max_z);

        std::vector<int> solve2d(int a, int b, int c, int k);

        int MCD(int x, int y);

        bool test2d(int a, int b, int c, int k);

        int getX();
        int getY();
        int getZ();


};


#endif //COIOTE_HEURISTIC_DIOPHANTINE_SOLVER_H
