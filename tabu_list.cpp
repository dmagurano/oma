#include "group.h"
#include "tabu_list.h"
//
// Created by stg on 22/12/2016.
//

tabu_list::tabu_list(int length) {
    this->max_length = length;
    this->counter = 0;
    this->queue = malloc(sizeof(tabu_move) * max_length);
}


void tabu_list::add_move(group *group1, group *group2) {
    tabu_move mov0 = tabu_move(group1, group2);
    add_move(mov0);
}

void tabu_list::add_move(tabu_move move0) {
    tabu_move *old = queue[counter % max_length];
    queue[counter % max_length] = move0;
    if (old != nullptr)
        free(old);
    ++counter;
}

bool tabu_list::check_move(group group1, group group2) {
    tabu_move *rule;
    for (int i = 0; i < max_length; ++i) {
        rule = queue[i];
        if ((*rule).test(group1, group2))
            return true; //move is tabu
    }
    return false; //move is not tabu
}

bool tabu_move::test(group group1, group group2) {
    return (group1 == *this->group1 && group2 == *this->group2) ||
            (group2 == *this->group1 && group1 == *this->group2);
}
