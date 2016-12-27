#include "group.h"
#include "tabu_list.h"
//
// Created by stg on 22/12/2016.
//

tabu_list::tabu_list(int length) {
    this->max_length = length;
    this->queue = std::vector<tabu_move>();
}


void tabu_list::add_move(group *group1, group *group2) {
    tabu_move mov0 = tabu_move(group1, group2);
    add_move(mov0);
}

void tabu_list::add_move(tabu_move move0) {
    this->queue.insert(this->queue.end(), move0);
    if (this->queue.size() > max_length)
    {
        this->queue.erase(this->queue.begin());
    }
}

bool tabu_list::check_move(group group1, group group2) {
    tabu_move *rule;
    std::vector<tabu_move>::iterator it;
    for (it = this->queue.begin(); it < this->queue.end(); it++) {
        rule = &(*it);
        if ((*rule).test(group1, group2))
            return true; //move is tabu
    }
    return false; //move is not tabu
}

