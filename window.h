//
// Created by Francesco on 03/01/2017.
//

#ifndef COIOTE_HEURISTIC_WINDOW_H
#define COIOTE_HEURISTIC_WINDOW_H


/*
 * Questa classe serve per ottenere finestre di valori partendo da x0 fino a lower bound e upper bound inclusi
 */

class window
{
private:
    int x0;
    int current_width;
    bool left_point;
    int lower_bound;
    int upper_bound;
    int valid_values;

    int getIndex();

public:
    window(int x0, int lb, int up);
    int getNextIndex();
    bool hasNext();

    bool canGetLeft();
    bool canGetRight();

    int* getArray();
    int getArrayLenght();
    void reset();
    int getWidth();

};


#endif //COIOTE_HEURISTIC_WINDOW_H
