//
// Created by danie on 30/11/2016.
//

#ifndef COIOTE_HEURISTIC_PROVA_H
#define COIOTE_HEURISTIC_PROVA_H


class solutionArray {

private:

    int *solArray;
    int iCells; int jCells; int nCustomerTypes; int timeSteps;

public:

    solutionArray(int**** sol, int iCells, int jCells, int nCustomerTypes, int timeSteps){

        this->iCells = iCells;
        this->jCells = jCells;
        this->nCustomerTypes = nCustomerTypes;
        this->timeSteps = timeSteps;

        int size = iCells*jCells*nCustomerTypes*timeSteps;
        solArray = new int[size];

        for(int i=0; i<iCells; i++)
            for(int j=0; j<jCells; j++)
                for(int k=0; k<nCustomerTypes; k++)
                    for(int t=0; t<timeSteps; t++)
                        solArray[i + j*jCells + k*jCells*nCustomerTypes + t*jCells*nCustomerTypes*timeSteps] = sol[i][j][k][t];

    }

    int & getElement( int i,  int j,  int k, int t) const {
        return this->solArray[i + j*jCells + k*jCells*nCustomerTypes + t*jCells*nCustomerTypes*timeSteps];
    }

    int * getSolArray() { return this->solArray; }

};


#endif //COIOTE_HEURISTIC_PROVA_H
