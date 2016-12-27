//
// Created by Francesco on 22/12/2016.
//

#ifndef COIOTE_HEURISTIC_GROUP_H
#define COIOTE_HEURISTIC_GROUP_H

#include <stdlib.h>


class group
{
private:
    int source_cell;
    int type_of_people;
    int usercapability;
public:
    int getUsercapability() const;

    void setUsercapability(int usercapability);

    int getDestination_cell() const;

    void setDestination_cell(int destination_cell);

private:
    int time_step;
    int group_capability;
    int destination_cell;
    int xj; //numero di persone di tipo "type of people" al tempo time_step presi dalla cella source_cell

public:
    group(int mcm, int sc, int dc, int top, int usercapability, int ts, int number_of_people_in_cell);

    group(group group1, int mcm, int source_cell, int dc, int type, int capability, int time, int person_in_cell); //crea un gruppo simile però da un altra cella calcolata in base alla capacità del group passato come parametro

    float cost(int new_destination, int new_time, double ****costs);

    int getGroup_capability() const;

    void setGroup_capability(int group_capability);

    int getDc() const;

    void setDc(int dc);

    int getSource_cell() const;

    void setSource_cell(int source_cell);

    int getType_of_people() const;

    void setType_of_people(int type_of_people);

    int getTime_step() const;

    void setTime_step(int time_step);

    int getXj() const;

    void setXj(int xj);

    bool trim(int new_capability);

    bool operator==(const group &other);


};


#endif //COIOTE_HEURISTIC_GROUP_H
