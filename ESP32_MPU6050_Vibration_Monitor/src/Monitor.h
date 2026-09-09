#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>

class MPU6050
{
public:

    bool begin();

    bool read(
        int16_t &ax,
        int16_t &ay,
        int16_t &az,
        int16_t &temperature,
        int16_t &gx,
        int16_t &gy,
        int16_t &gz
    );

    float accelToG(int16_t raw);

    float gyroToDegPerSec(int16_t raw);

    float temperatureToC(int16_t raw);
};

#endif