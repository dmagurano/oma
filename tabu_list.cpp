#include "group.h"
#include "tabu_list.h"
//
// Created by stg on 22/12/2016.
//

tabu_list::tabu_list(int length) {
    this->max_length = length;
    this->counter = 0;
    this->head = nullptr;
    this->tail = nullptr;
}


void tabu_list::add_move(group *group1, group *group2) {
    tabu_move mov0 = tabu_move(group1, group2);
    add_move(mov0);
}

void tabu_list::add_move(tabu_move move0) {
    if (head == NULL) {
        head = &move0;
        //head->next_move = NULL;
        tail = &move0;
    }
    else {
        *tail->next_move = move0;
        tail = &move0;
    }
    ++counter;
    if (counter > max_length)
        chop();
}

void tabu_list::chop() {
    tabu_move *chopped = head;
    *head = *head->next_move;
    //TODO: chopped should be deleted
    --counter;
}

bool tabu_list::check_move(group group1, group group2) {
    tabu_move *rule = head;
    while (rule != nullptr) {
        if ((*rule).test(group1, group2))
            return true; //move is tabu
        rule = rule->next_move;
    }
    return false; //move is not tabu
}

bool tabu_move::test(group group1, group group2) {
    return (group1 == *this->group1 && group2 == *this->group2) ||
            (group2 == *this->group1 && group1 == *this->group2);
}
