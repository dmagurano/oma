//
// Created by stg on 22/12/2016.
//

#ifndef COIOTE_HEURISTIC_TABU_LIST_H
#define COIOTE_HEURISTIC_TABU_LIST_H

#include "group.h"
#include "tabu_move.h"

class tabu_list
{
private:
    int max_length;
    int counter;
    tabu_move *head, *tail;

    void add_move(tabu_move move0);

public:

    tabu_list(int length);

    void add_move(group *group1, group *group2);

    bool check_move(group group1, group group2);

    void chop();
};

#endif //COIOTE_HEURISTIC_TABU_LIST_H
