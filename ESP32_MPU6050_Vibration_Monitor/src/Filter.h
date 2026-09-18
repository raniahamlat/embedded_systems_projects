#ifndef FILTER_H
#define FILTER_H

class MovingAverage
{
private:

    float *buffer;

    int size;
    int index;
    int count;

public:

    MovingAverage(float *storage, int bufferSize);

    float update(float value);
};

#endif