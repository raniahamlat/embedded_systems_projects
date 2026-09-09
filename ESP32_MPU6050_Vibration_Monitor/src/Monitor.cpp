#include "Monitor.h"


// ========================================
// Constructor
// ========================================

Monitor::Monitor()
{
    // ====================================
    // Initial states
    // ====================================

    mpuState = NORMAL;

    bmeState = NORMAL;

    systemState = NORMAL;


    // ====================================
    // Sensor health
    // ====================================

    mpuHealthy = false;

    bmeHealthy = false;


    // ====================================
    // MPU thresholds
    // ====================================

    vibrationWarning = 0.20;

    vibrationAlarm = 0.50;

    gyroWarning = 100.0;

    gyroAlarm = 200.0;


    // ====================================
    // MPU persistence times
    // ====================================

    warningTime = 100;

    alarmTime = 200;

    warningStart = 0;

    alarmStart = 0;

    warningTiming = false;

    alarmTiming = false;


    // ====================================
    // BME thresholds
    // ====================================

    temperatureWarning = 50.0;

    temperatureAlarm = 70.0;

    humidityWarning = 70.0;

    humidityAlarm = 85.0;

    pressureWarning = 950.0;

    pressureAlarm = 900.0;


    // ====================================
    // BME persistence
    // ====================================

    bmeWarningStart = 0;

    bmeAlarmStart = 0;

    bmeWarningTiming = false;

    bmeAlarmTiming = false;
}


// ========================================
// MPU6050 monitoring
// ========================================

void Monitor::update(
    float dynamicAcceleration,
    float gyroMagnitude,
    unsigned long currentTime
)
{
    bool warningCondition =
        dynamicAcceleration >= vibrationWarning ||
        gyroMagnitude >= gyroWarning;


    bool alarmCondition =
        dynamicAcceleration >= vibrationAlarm ||
        gyroMagnitude >= gyroAlarm;


    // ====================================
    // Alarm
    // ====================================

    if (alarmCondition)
    {
        if (!alarmTiming)
        {
            alarmTiming = true;

            alarmStart = currentTime;
        }


        if (currentTime - alarmStart >= alarmTime)
        {
            mpuState = ALARM;
        }
    }
    else
    {
        alarmTiming = false;

        alarmStart = 0;
    }


    // ====================================
    // Warning
    // ====================================

    if (warningCondition && !alarmCondition)
    {
        if (!warningTiming)
        {
            warningTiming = true;

            warningStart = currentTime;
        }


        if (currentTime - warningStart >= warningTime)
        {
            mpuState = WARNING;
        }
    }
    else
    {
        warningTiming = false;

        warningStart = 0;
    }


    // ====================================
    // Normal
    // ====================================

    if (!warningCondition && !alarmCondition)
    {
        mpuState = NORMAL;
    }


    // ====================================
    // Calculate final system state
    // ====================================

    updateSystemState();
}


// ========================================
// BME280 monitoring
// ========================================

void Monitor::updateBME(
    float temperature,
    float humidity,
    float pressure,
    unsigned long currentTime
)
{
    // ====================================
    // Warning conditions
    // ====================================

    bool temperatureWarningCondition =
        temperature >= temperatureWarning;

    bool humidityWarningCondition =
        humidity >= humidityWarning;

    bool pressureWarningCondition =
        pressure <= pressureWarning;


    bool warningCondition =
        temperatureWarningCondition ||
        humidityWarningCondition ||
        pressureWarningCondition;


    // ====================================
    // Alarm conditions
    // ====================================

    bool temperatureAlarmCondition =
        temperature >= temperatureAlarm;

    bool humidityAlarmCondition =
        humidity >= humidityAlarm;

    bool pressureAlarmCondition =
        pressure <= pressureAlarm;


    bool alarmCondition =
        temperatureAlarmCondition ||
        humidityAlarmCondition ||
        pressureAlarmCondition;


    // ====================================
    // Alarm
    // ====================================

    if (alarmCondition)
    {
        if (!bmeAlarmTiming)
        {
            bmeAlarmTiming = true;

            bmeAlarmStart = currentTime;
        }


        if (currentTime - bmeAlarmStart >= alarmTime)
        {
            bmeState = ALARM;
        }
    }
    else
    {
        bmeAlarmTiming = false;

        bmeAlarmStart = 0;
    }


    // ====================================
    // Warning
    // ====================================

    if (warningCondition && !alarmCondition)
    {
        if (!bmeWarningTiming)
        {
            bmeWarningTiming = true;

            bmeWarningStart = currentTime;
        }


        if (currentTime - bmeWarningStart >= warningTime)
        {
            bmeState = WARNING;
        }
    }
    else
    {
        bmeWarningTiming = false;

        bmeWarningStart = 0;
    }


    // ====================================
    // Normal
    // ====================================

    if (!warningCondition && !alarmCondition)
    {
        bmeState = NORMAL;
    }


    // ====================================
    // Calculate final system state
    // ====================================

    updateSystemState();
}


// ========================================
// MPU health
// ========================================

void Monitor::setMPUHealth(bool healthy)
{
    mpuHealthy = healthy;

    if (!healthy)
    {
        mpuState = FAULT;
    }
}


// ========================================
// BME health
// ========================================

void Monitor::setBMEHealth(bool healthy)
{
    bmeHealthy = healthy;

    if (!healthy)
    {
        bmeState = FAULT;
    }
}


// ========================================
// MPU health getter
// ========================================

bool Monitor::isMPUHealthy()
{
    return mpuHealthy;
}


// ========================================
// BME health getter
// ========================================

bool Monitor::isBMEHealthy()
{
    return bmeHealthy;
}


// ========================================
// Worst-case state arbitration
// ========================================

void Monitor::updateSystemState()
{
    // ====================================
    // FAULT has highest priority
    // ====================================

    if (!mpuHealthy ||
        !bmeHealthy)
    {
        systemState = FAULT;

        return;
    }


    // ====================================
    // ALARM
    // ====================================

    if (mpuState == ALARM ||
        bmeState == ALARM)
    {
        systemState = ALARM;

        return;
    }


    // ====================================
    // WARNING
    // ====================================

    if (mpuState == WARNING ||
        bmeState == WARNING)
    {
        systemState = WARNING;

        return;
    }


    // ====================================
    // NORMAL
    // ====================================

    systemState = NORMAL;
}


// ========================================
// Final system state
// ========================================

SystemState Monitor::getState()
{
    return systemState;
}


// ========================================
// MPU state
// ========================================

SystemState Monitor::getMPUState()
{
    return mpuState;
}


// ========================================
// BME state
// ========================================

SystemState Monitor::getBMEState()
{
    return bmeState;
}


// ========================================
// Final system state name
// ========================================

const char* Monitor::getStateName()
{
    switch (systemState)
    {
        case NORMAL:
            return "NORMAL";

        case WARNING:
            return "WARNING";

        case ALARM:
            return "ALARM";

        case FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}


// ========================================
// MPU state name
// ========================================

const char* Monitor::getMPUStateName()
{
    switch (mpuState)
    {
        case NORMAL:
            return "NORMAL";

        case WARNING:
            return "WARNING";

        case ALARM:
            return "ALARM";

        case FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}


// ========================================
// BME state name
// ========================================

const char* Monitor::getBMEStateName()
{
    switch (bmeState)
    {
        case NORMAL:
            return "NORMAL";

        case WARNING:
            return "WARNING";

        case ALARM:
            return "ALARM";

        case FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}
