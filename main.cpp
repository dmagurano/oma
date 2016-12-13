#include <iostream>
#include "diophantine_solver.h"
#include <stdlib.h>
#include <time.h>



using namespace std;

int main() {
    int i;

    bool stop = false;
    srand(time(NULL));
    int k, m;
    int tentativi = 0;
    k = 1000;
    m = 1000;
    for (i = 1; i < 32+20*2+42*3; i++) {
        diophantine_solver ds = diophantine_solver(1, 2, 3, i);
        ds.solve(&m, &k, 32, 20, 42);
        //cout << "Provo con m = " << m << " e k = " << k << " e demand " << i << endl;
        cout << "Soluzione: x = " << ds.getX() << " y = " << ds.getY() << " z = " << ds.getZ() << endl;
    }
/*    int b, c, k, i;

    for (int j = 1; j < 100; j++)
    {
        b = 7;
        c = 5;
        k = 2;
        if (c % ds.MCD(j, b) == 0)
        {
            i = j;

            std::cout << "Solving " << j << " * x + " << b << " * y = " << c << std::endl;
            if (ds.test(i, b, c, k))
                std::cout << "Resolved!  x0 = " << ds.solve2d(i, b, c, k).data()[0] << "  y0 = " << ds.solve2d(i, b, c, k).data()[1] << std::endl;
            else
                std::cout << "Non resolved..." << std::endl;
        }
    }*/
}