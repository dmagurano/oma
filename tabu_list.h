//
// Created by stg on 22/12/2016.
//

#ifndef COIOTE_HEURISTIC_TABU_LIST_H
#define COIOTE_HEURISTIC_TABU_LIST_H

#include "group.h"

class tabu_list
{
private:
    int max_length;
    int counter;
    move *head, *tail;

    void add_move(move move0) ;

public:

    tabu_list(int length);

    void add_move(group *group1, group *group2) ;

    bool check_move(group group1, group group2) ;

    void chop();
};

class move {
private:
    group *group1, *group2;
public:
    move *next_move;

    move(group *group1, group *group2);

    bool test(group group1, group group2);
};

#endif //COIOTE_HEURISTIC_TABU_LIST_H
