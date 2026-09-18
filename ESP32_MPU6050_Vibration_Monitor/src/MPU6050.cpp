#include "MPU6050.h"
#include <Wire.h>

#define MPU6050_ADDR  0x68
#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT_H  0x3B


bool MPU6050::begin()
{
    Wire.begin();

    Wire.beginTransmission(MPU6050_ADDR);

    Wire.write(PWR_MGMT_1);
    Wire.write(0x00);

    return Wire.endTransmission() == 0;
}


bool MPU6050::read(
    int16_t &ax,
    int16_t &ay,
    int16_t &az,
    int16_t &temperature,
    int16_t &gx,
    int16_t &gy,
    int16_t &gz
)
{
    Wire.beginTransmission(MPU6050_ADDR);

    Wire.write(ACCEL_XOUT_H);

    if (Wire.endTransmission(false) != 0)
    {
        return false;
    }

    Wire.requestFrom(MPU6050_ADDR, 14);

    if (Wire.available() != 14)
    {
        return false;
    }


    ax = (int16_t)((Wire.read() << 8) | Wire.read());

    ay = (int16_t)((Wire.read() << 8) | Wire.read());

    az = (int16_t)((Wire.read() << 8) | Wire.read());

    temperature =
        (int16_t)((Wire.read() << 8) | Wire.read());

    gx = (int16_t)((Wire.read() << 8) | Wire.read());

    gy = (int16_t)((Wire.read() << 8) | Wire.read());

    gz = (int16_t)((Wire.read() << 8) | Wire.read());

    return true;
}


float MPU6050::accelToG(int16_t raw)
{
    return raw / 16384.0;
}


float MPU6050::gyroToDegPerSec(int16_t raw)
{
    return raw / 131.0;
}


float MPU6050::temperatureToC(int16_t raw)
{
    return (raw / 340.0) + 36.53;
}