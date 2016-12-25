//
// Created by Francesco on 23/12/2016.
//

#include <cstdlib>
#include <cmath>
#include <random>
#include "simulated_annealing.h"

simulated_annealing::simulated_annealing(float cooling_parameter, float T0, int plateau_lenght, float curr_objective_function) : cooling_parameter(
        cooling_parameter), T0(T0), plateau_lenght(plateau_lenght)
{
    this->T0 = T0;
    this->cooling_parameter = cooling_parameter;
    this->plateau_lenght = plateau_lenght;
    this->iteration_counter = 0;
    this->k = 0;
    this->current_temperature = T0;
    this->current_objective_function = curr_objective_function;
}

bool simulated_annealing::accept_solution(float new_solution_obj_funct)
{
    bool accept = false;
    if (new_solution_obj_funct < current_objective_function)
    {
        current_objective_function = new_solution_obj_funct;
        accept = true;
    }
    else
    {
        double accepting_probability = exp((-new_solution_obj_funct - current_objective_function) / current_temperature);

        double probability = ((double) (rand() % 101)) / 100;
        if (probability < accepting_probability)
        {
            accept = true;
            //current_objective_function = new_solution_obj_funct;
        }
    }
    this->iteration_counter++;
    if (this->iteration_counter > this->plateau_lenght)
    {
        this->k++;
        this->plateau_lenght += plateau_lenght;
        this->current_temperature = pow(this->cooling_parameter, k) * this->T0;
    }
    return accept;
}

int simulated_annealing::getIteration_counter() const {
    return iteration_counter;
}

void simulated_annealing::setIteration_counter(int iteration_counter) {
    simulated_annealing::iteration_counter = iteration_counter;
}

float simulated_annealing::getCurrent_temperature() const {
    return current_temperature;
}

void simulated_annealing::setCurrent_temperature(float current_temperature) {
    simulated_annealing::current_temperature = current_temperature;
}

float simulated_annealing::getCooling_parameter() const {
    return cooling_parameter;
}

void simulated_annealing::setCooling_parameter(float cooling_parameter) {
    simulated_annealing::cooling_parameter = cooling_parameter;
}

float simulated_annealing::getT0() const {
    return T0;
}

void simulated_annealing::setT0(float T0) {
    simulated_annealing::T0 = T0;
}

int simulated_annealing::getPlateau_lenght() const {
    return plateau_lenght;
}

void simulated_annealing::setPlateau_lenght(int plateau_lenght) {
    simulated_annealing::plateau_lenght = plateau_lenght;
}

float simulated_annealing::getCurrent_objective_function() const {
    return current_objective_function;
}

void simulated_annealing::setCurrent_objective_function(float current_objective_function) {
    simulated_annealing::current_objective_function = current_objective_function;
}
