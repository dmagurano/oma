//
// Created by Francesco on 12/12/2016.
//

#include "diophantine_solver.h"




diophantine_solver::diophantine_solver(int a, int b, int c, int d){
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->x.weight = 0.33;
    this->y.weight = 0.33;
    this->z.weight = 0.33;
    this->max_tentativi = 10000;
    solved = false;
    unsolvable = false;
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

int diophantine_solver::mcm(int a, int b)
{
    return ( (a * b) / MCD(a,b) );
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
    // se anche mandando tutti gli utenti non si riesce a soddisfare la richiesta l'equazione è irrisolvibile
    std::vector<int> temp2d;
    if (max_x * this->a + max_y * this->b + max_z * this->c < d)
    {
        // se la domanda è troppo alta manda tutti
        setX(max_x);
        setY(max_y);
        setZ(max_z);
        this->solved = true;
        //setIsSolvable(false);
        return temp2d;
    }
    x.max_value = max_x;
    y.max_value = max_y;
    z.max_value = max_z;
    //general solution computation
    int p = MCD(this->a, this->b);
    int a1, b1;
    a1 = this->a/p;
    b1 = this->b/p;

    int u0, v0;

    temp2d = solve2d(a1, b1, this->c, 2);

    if (this->unsolvable) return temp2d;

    v0 = temp2d.data()[1];
    u0 = temp2d.data()[0];

    int z0, t0;

    temp2d = solve2d(this->c, p, this->d, 2);

    if (this->unsolvable) return temp2d;

    t0 = temp2d.data()[1];
    z0 = temp2d.data()[0];

    int x0, y0;

    temp2d = solve2d(a1, b1, t0, 2);

    if (this->unsolvable) return temp2d;

    y0 = temp2d.data()[1];
    x0 = temp2d.data()[0];

    // initial solution generated
    temp2d = std::vector<int>();

    srand(time(NULL));
    int tentativi = 0;
    int new_k, new_m;

    srand(time(NULL));
    this->solved = false;
    do
    {
        // trying to get a good solution first with the initial k and m
        setX(x0 + b1 * *k - u0 * *m);
        setY(y0 - a1 * *k - v0 * *m);
        setZ(z0 + p * *m);



        if ((this->x.val >= 0 && this->x.val <= max_x) && (this->y.val >= 0 && this->y.val <= max_y) && (this->z.val >= 0 && this->z.val <= max_z))
        {
            this->solved = true;
            break;
        }
        else
        {
            this->solved = false;
        }
        //otherwise try to move to other two k and m which improve the solution
        //new_k = get_random_value_from_interval(*k, 20, direction_k);
        //new_m = get_random_value_from_interval(*m, 20, direction_m);
        int raggio = rand()%20;
        float bestscore = score();
        int bestteta  = 0;

        for (int teta = 0; teta < 360; teta++)
        {
            new_k = *k + raggio * cos ( teta * PI / 180.0);
            new_m = *m + raggio * sin ( teta * PI / 180.0);

            if (score(x0 + b1 * new_k - u0 * new_m, y0 - a1 * new_k - v0 * new_m, z0 + p * new_m, x.max_value,
                      y.max_value, z.max_value) < bestscore) {
                bestteta = teta;
                bestscore = score(x0 + b1 * new_k - u0 * new_m, y0 - a1 * new_k - v0 * new_m, z0 + p * new_m, x.max_value,
                                  y.max_value, z.max_value);
            }

        }
        if (bestscore != score())
        {
            *k += raggio * cos ( bestteta * PI / 180.0);
            *m += raggio * sin ( bestteta * PI / 180.0);
        }
        tentativi++;
    }
    while (! solved && tentativi < this->max_tentativi);
    if (this->solved)
    {
        temp2d.push_back(getX());
        temp2d.push_back(getY());
        temp2d.push_back(getZ());
    }
    return temp2d;
}

int diophantine_solver::get_random_value_from_interval(int x0, int r, bool negative)
{
    int r_offset = rand() % r + 1;
    r_offset *= negative ? -1 : 1;
    return x0 + r_offset;
}

float diophantine_solver::score()
{
    return score(getX(), getY(), getZ(), x.max_value, y.max_value, z.max_value);
}

bool diophantine_solver::isSolved()
{
    return this->solved;
}

void diophantine_solver::setIsSolvable(bool solvable) {
    this->unsolvable = ! solvable;
    if (this->unsolvable)
    {
        this->solved = false;
    }
}

float diophantine_solver::score(int x, int y, int z, int max_x, int max_y, int max_z)
{
    float score_x, score_y, score_z, score;
    if (x < 0 || x > max_x)
    {
        if (x < 0)
        {
            score_x = -x;
        }
        else
        {
            score_x = x - max_x;
        }
    }

    if (y < 0 || y > max_y)
    {
        if (y < 0)
        {
            score_y = -y;
        }
        else
        {
            score_y = y - max_y;
        }
    }

    if (z < 0 || z > max_z)
    {
        if (z < 0)
        {
            score_z = -z;
        }
        else
        {
            score_z = z - max_z;
        }
    }

    score = score_x * this->x.weight + score_y * this->y.weight + score_z * this->z.weight;

    if (score < 0)
    {
        score *= -1;
    }
    return score;
}

void diophantine_solver::setX(int x)
{
    this->solved = true;
    this->x.val = x;
}

void diophantine_solver::setY(int y)
{
    this->solved = true;
    this->y.val = y;
}

void diophantine_solver::setZ(int z)
{
    this->solved = true;
    this->z.val = z;
}

int diophantine_solver::getX()
{
    return this->x.val;
}

int diophantine_solver::getY()
{
    return this->y.val;
}

int diophantine_solver::getZ()
{
    return this->z.val;
}

void diophantine_solver::setWeightX(float w)
{
    this->x.weight = w;
}

void diophantine_solver::setWeightY(float w)
{
    this->y.weight = w ;
}

void diophantine_solver::setWeightZ(float w)
{
    this->z.weight = w ;
}

void diophantine_solver::setTentativi(int t)
{
    this->max_tentativi = t;
}

/* https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
 * https://en.wikipedia.org/wiki/Euclidean_algorithm
 */
std::vector<int> diophantine_solver::solve2d(int a, int b, int c, int k)
{
    std::vector<int> result = std::vector<int>();
    int g = MCD(a, b);
    if (c % g != 0) {
        setIsSolvable(false);
        return result;
    }
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

