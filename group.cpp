//
// Created by Francesco on 22/12/2016.
//

#include "group.h"
#include "solutionArray.h"

group::group(int mcm, int sc, int top, int usercapability, int ts, int number_of_people_in_cell)
{
    this->source_cell = sc;
    this->type_of_people = top;
    this->time_step = ts;
    int min, max;
    min = usercapability;
    max = usercapability * number_of_people_in_cell;
    xj = rand() % (max - min) + min;
    xj -= (xj * usercapability % (mcm/usercapability))/usercapability;
    this->group_capability = xj * usercapability;
}

int group::getSource_cell() const {
    return source_cell;
}

void group::setSource_cell(int source_cell) {
    group::source_cell = source_cell;
}

int group::getType_of_people() const {
    return type_of_people;
}

void group::setType_of_people(int type_of_people) {
    group::type_of_people = type_of_people;
}

int group::getTime_step() const {
    return time_step;
}

void group::setTime_step(int time_step) {
    group::time_step = time_step;
}

int group::getXj() const {
    return xj;
}

void group::setXj(int xj) {
    group::xj = xj;
}
