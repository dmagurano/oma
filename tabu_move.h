//
// Created by Francesco on 25/12/2016.
//

#ifndef COIOTE_HEURISTIC_MOVE_H
#define COIOTE_HEURISTIC_MOVE_H

#include "group.h"


class tabu_move
{
private:
    group *group1, *group2;
public:
    tabu_move *next_move;

    tabu_move(group *group1, group *group2);

    bool test(group group1, group group2);

};


#endif //COIOTE_HEURISTIC_MOVE_H
