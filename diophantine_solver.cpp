//
// Created by Francesco on 12/12/2016.
//

#include "diophantine_solver.h"




diophantine_solver::diophantine_solver(int a, int b, int c, int d){
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
}

int diophantine_solver::MCD(int x, int y){
    if (x == 0 && y == 0)
        return 0;
    int a = x > 0 ? x : x * -1; // valore assoluto
    int b = y > 0 ? y : y * -1; // valore assoluto

    if (b > a)
    {
        // fast int swap
        a = a + b; // a <- a + b
        b = a - b; // b <- a + b - b
        a = a - b; // a <- a + b - a
    }
    int resto = -1;
    while (resto != 0)
    {
        resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}


/*
Let p = GCD(a,b), a' = a/p, b' = b/p
 Let u0 and v0 any solution of a'u + b'v = c
 z0, t0 any solution of cz + pt = d
 x0, y0 any solution of a'x + b'y = t0
 The general solution of ax + by + cz = d is :
 x = x0 + b'k - u0m
 y = y0 - a'k - v0m
 z = z0 + pm with k and m any integer in Z


*/
std::vector<int> diophantine_solver::solve(int *k, int *m, int max_x, int max_y, int max_z)
{
    int p = MCD(this->a, this->b);
    int a1, b1;
    a1 = this->a/p;
    b1 = this->b/p;

    int u0, v0;

    std::vector<int> temp2d = solve2d(a1, b1, this->c, 2);

    v0 = temp2d.data()[1];
    u0 = temp2d.data()[0];

    int z0, t0;

    temp2d = solve2d(this->c, p, this->d, 2);

    t0 = temp2d.data()[1];
    z0 = temp2d.data()[0];

    int x0, y0;

    temp2d = solve2d(a1, b1, t0, 2);

    y0 = temp2d.data()[1];
    x0 = temp2d.data()[0];

    temp2d = std::vector<int>();

    srand(time(NULL));
    int tentativi = 0;

    do
    {
        this->x = x0 + b1 * *k - u0 * *m;
        this->y = y0 - a1 * *k - v0 * *m;
        this->z = z0 + p * *m;

        *k = rand() % ((max_x + max_y + max_z) > 100? max_x + max_y + max_z : 100);
        *m = rand() % ((max_x + max_y + max_z) > 100? max_x + max_y + max_z : 100);

        if (rand()%101 > 50)
            *k *= -1;

        if (rand()%101 > 50)
            *m *= -1;

        tentativi++;
        if (this->x >= 0 && this->x <= max_x)
        {
            if (this->y >= 0 && this->y <= max_y)
            {
                if (this->z >= 0 && this->z <= max_z)
                {
                    temp2d.push_back(x);
                    temp2d.push_back(y);
                    temp2d.push_back(z);
                    break;
                }
            }
        }
    }
    while (tentativi < 1000);

    return temp2d;
}

int diophantine_solver::getX()
{
    return this->x;
}

int diophantine_solver::getY()
{
    return this->y;
}

int diophantine_solver::getZ()
{
    return this->z;
}

/* https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
 * https://en.wikipedia.org/wiki/Euclidean_algorithm
 */
std::vector<int> diophantine_solver::solve2d(int a, int b, int c, int k)
{
    std::vector<int> result = std::vector<int>();
    int g = MCD(a, b);
    if (c % g != 0)
        return  result;

    int a1, b1, c1;

    a1 = a / g;
    b1 = b / g;
    c1 = c / g;

    //find bezous coefficent
    int quotient, s, t, r, old_s, old_t, old_r, temp;
    s = 0;
    t = 1;
    r = b1;
    old_s = 1;
    old_t = 0;
    old_r = a;
    while (r != 0)
    {
        quotient = old_r / r;
        temp = r;
        r = old_r - quotient * r;
        old_r = temp;

        temp = s;
        s = old_s - quotient * s;
        old_s = temp;

        temp = t;
        t = old_t - quotient * t;
        old_t = temp;
    }
    // coefficenti di Bezout old s e old r
    int x0, y0;

    x0 = old_s * (c1 / g);
    y0 = old_t * (c1 / g);

    result.push_back(x0);
    result.push_back(y0);

    return result;
}

bool diophantine_solver::test2d(int a, int b, int c, int k)
{
    int x0, y0;
    std::vector<int> temp2d = solve2d(a, b, c, k);

    y0 = temp2d.data()[1];
    x0 = temp2d.data()[0];

    return a * x0 + b * y0 == c;
}

