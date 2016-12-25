//
// Created by Francesco on 23/12/2016.
//

#ifndef COIOTE_HEURISTIC_SIMULATED_ANNELING_H
#define COIOTE_HEURISTIC_SIMULATED_ANNELING_H


class simulated_annealing {

private:
    int iteration_counter;
    float current_temperature;
    float current_objective_function;
    float cooling_parameter;
    float T0;
    /*
     *  Plateau lenght:
     *  number of iteration to wait before cooling
     *  proportional to neighboorhoodsize
     *
     */
    int plateau_lenght;
    int k;
public:
    simulated_annealing(float cooling_parameter, float T0, int plateau_lenght, float curr_objective_function);

    bool accept_solution(float new_solution_obj_funct);

    float getCurrent_objective_function() const;

    void setCurrent_objective_function(float current_objective_function);

    int getIteration_counter() const;

    void setIteration_counter(int iteration_counter);

    float getCurrent_temperature() const;

    void setCurrent_temperature(float current_temperature);

    float getCooling_parameter() const;

    void setCooling_parameter(float cooling_parameter);

    float getT0() const;

    void setT0(float T0);

    int getPlateau_lenght() const;

    void setPlateau_lenght(int plateau_lenght);

};


#endif //COIOTE_HEURISTIC_SIMULATED_ANNELING_H
