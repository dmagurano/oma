//
// Created by Francesco on 22/12/2016.
//
#include <stdio.h>
#include "group.h"
#include "solutionArray.h"

group::group(int mcm, int sc, int dc, int top, int usercapability, int ts, int number_of_people_in_cell)
{
    this->source_cell = sc;
    this->type_of_people = top;
    this->time_step = ts;
    this->destination_cell = dc;
    this->usercapability = usercapability;
    int min, max, alpha;
    //calculating alpha, where xj = alpha * mcm / user_cap
    min = 1;
    max = (number_of_people_in_cell * usercapability) / mcm;
    if (max == 0) {
        //TODO exception
        printf("MAX IS 0");
    }
    if (min == max)
        alpha = 1;
    else
        alpha = rand() % (max - min) + min;
    xj = alpha * mcm / usercapability;
    this->group_capability = xj * usercapability;
}

group::group(group group1, int mcm, int source_cell, int dc, int type, int capability, int time, int person_in_cell)
{
    this->source_cell = source_cell;
    this->destination_cell = dc;
    this->type_of_people = type;
    this->time_step = time;
    this->usercapability = capability;
    this->xj = group1.getGroup_capability() / capability;
    if (xj > person_in_cell)
    {
        //trimming
        int gc = person_in_cell * usercapability;
        gc -= gc % mcm;
        xj = gc / usercapability;
        /*xj -= (xj * usercapability % (mcm/usercapability))/usercapability;*/
        if (!group1.trim(gc)) {
            //TODO exception
            printf("COULD NOT TRIM");
        }
    }
    this->group_capability = xj * usercapability;

}

bool group::trim(int new_capability)
{
    int new_xj = new_capability / getUsercapability();
    if (new_xj < 1)
        return false;
    setXj(new_xj);
    setGroup_capability(new_capability);
    return true;
}

float group::cost(int new_destination, int new_time, double ****costs)
{
    //cost is given by new_cost - old_cost
    return (getXj() * costs[getSource_cell()][new_destination][getType_of_people()][new_time]) - (getXj() * costs[getSource_cell()][getDestination_cell()][getType_of_people()][getTime_step()]);
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

int group::getGroup_capability() const {
    return group_capability;
}

void group::setGroup_capability(int group_capability) {
    group::group_capability = group_capability;
}

int group::getDc() const {
    return destination_cell;
}

void group::setDc(int dc) {
    group::destination_cell = dc;
}

int group::getUsercapability() const {
    return usercapability;
}

void group::setUsercapability(int usercapability) {
    group::usercapability = usercapability;
}

int group::getDestination_cell() const {
    return destination_cell;
}

void group::setDestination_cell(int destination_cell) {
    group::destination_cell = destination_cell;
}

bool group::operator==(const group &other) {
    return (this->time_step == other.getTime_step() && this->destination_cell == other.getDestination_cell() &&
            this->source_cell == other.getSource_cell() &&
            this->type_of_people == other.getType_of_people()); //TODO: try w/o this last one
}
