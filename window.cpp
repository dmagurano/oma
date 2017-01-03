//
// Created by Francesco on 03/01/2017.
//

#include <cstdlib>
#include "window.h"


window::window(int x0, int lb, int up)
{
    this->x0 = x0;
    if (lb > up)
    {
        int temp = lb;
        lb = up;
        up = temp;
    }
    this->lower_bound = lb;
    this->upper_bound = up;
    this->current_width = 1;
    this->left_point = false;
    this->valid_values = 0;
}

void window::reset()
{
    this->current_width = 1;
    this->left_point = false;
}

int window::getNextIndex()
{
    int t = lower_bound - 1;
    if (hasNext())
    {
        do
        {
            t = getIndex();
        } while (t < lower_bound);

    }
    return t;
}

int window::getIndex()
{
    int result = lower_bound - 1;
    if (left_point)
    {
        // sinistra
        if (canGetLeft())
        {
            result = x0 - current_width;
        }
    }
    else
    {
        // destra
        if (canGetRight())
        {
            result = x0 + current_width;
        }
    }


    if (left_point)
    {
        current_width++;
    }
    left_point = ! left_point;
    return result;
}

bool window::hasNext()
{
    if (!canGetLeft() && !canGetRight())
        return false;
    return true;
}

bool window::canGetLeft()
{
    return x0 - current_width >= lower_bound;
}

bool window::canGetRight()
{
    return x0 + current_width <= upper_bound;
}

int window::getArrayLenght()
{
    return this->valid_values;
}

int* window::getArray()
{
    reset();
    while (hasNext())
    {
        if (getNextIndex() >= lower_bound) valid_values++;
    }
    int* result = (int*) malloc(sizeof(int) * valid_values);
    int temp_index = 0, t;
    reset();
    while (hasNext())
    {
        t = getNextIndex();
        if (t >= lower_bound)
            result[temp_index++] = t;
    }
    return result;
}

int window::getWidth()
{
    return this->current_width;
}

