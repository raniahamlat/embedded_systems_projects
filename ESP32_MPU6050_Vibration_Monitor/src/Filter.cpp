#include "Filter.h"


MovingAverage::MovingAverage(
    float *storage,
    int bufferSize
)
{
    buffer = storage;

    size = bufferSize;

    index = 0;

    count = 0;


    for (int i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }
}


float MovingAverage::update(float value)
{
    buffer[index] = value;

    index++;

    if (index >= size)
    {
        index = 0;
    }


    if (count < size)
    {
        count++;
    }


    float sum = 0;

    for (int i = 0; i < count; i++)
    {
        sum += buffer[i];
    }


    return sum / count;
}