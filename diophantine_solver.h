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
    struct Variable
    {
        int val;
        int max_value;
        float weight;
    };

    typedef  Variable variab;

    variab x, y, z;

    int max_tentativi;
    bool solved;

    std::vector<int> solve2d(int a, int b, int c, int k);

    int MCD(int x, int y);

    bool test2d(int a, int b, int c, int k);

    int get_random_value_from_interval(int x0, int r, bool negative);

    void setX(int x);
    void setY(int y);
    void setZ(int z);

    public:
        diophantine_solver(int a, int b, int c, int d);

        std::vector<int> solve(int *m, int *k, int max_x, int max_y, int max_z);

        int getX();
        int getY();
        int getZ();

        void setWeightX(float w);
        void setWeightY(float w);
        void setWeightZ(float w);

        bool isSolved();

        //optimization functions
        float score();
        float score(int x, int y, int z, int max_x, int max_y, int max_z);
        void setTentativi(int t);


};


#endif //COIOTE_HEURISTIC_DIOPHANTINE_SOLVER_H
