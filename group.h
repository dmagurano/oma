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
    int time_step;
    int group_capability;
    int xj; //numero di persone di tipo "type of people" al tempo time_step presi dalla cella source_cell

public:
    group(int mcm, int sc, int top, int ts);

    group(group group1); //crea un gruppo simile però da un altra cella calcolata in base alla capacità del group passato come parametro

    int getSource_cell() const;

    void setSource_cell(int source_cell);

    int getType_of_people() const;

    void setType_of_people(int type_of_people);

    int getTime_step() const;

    void setTime_step(int time_step);

    int getXj() const;

    void setXj(int xj);

    bool operator==(const group &other) const;


};


#endif //COIOTE_HEURISTIC_GROUP_H
