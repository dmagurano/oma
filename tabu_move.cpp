//
// Created by Francesco on 25/12/2016.
//

#include "tabu_move.h"

tabu_move::tabu_move(group *group1, group *group2) {
    this->group1 = group1;
    this->group2 = group2;
}

group *tabu_move::getGroup1() const {
    return group1;
}

void tabu_move::setGroup1(group *group1) {
    tabu_move::group1 = group1;
}

group *tabu_move::getGroup2() const {
    return group2;
}

void tabu_move::setGroup2(group *group2) {
    tabu_move::group2 = group2;
}

bool tabu_move::test(group group1, group group2) {
    return (group1 == *this->group1 && group2 == *this->group2) ||
           (group2 == *this->group1 && group1 == *this->group2);
}
