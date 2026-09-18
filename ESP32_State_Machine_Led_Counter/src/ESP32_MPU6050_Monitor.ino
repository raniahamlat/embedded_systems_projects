#include <math.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

#include "Filter.h"
#include "MPU6050.h"
#include "Monitor.h"


// ============================================================
// PIN CONFIGURATION
// ============================================================

#define SDA_PIN 21
#define SCL_PIN 22

#define BME280_ADDRESS 0x76

#define LED_NORMAL 25
#define LED_WARNING 26
#define LED_ALARM 27


// ============================================================
// SAMPLING CONFIGURATION
// ============================================================

const unsigned long SAMPLE_INTERVAL = 10;   // 10 ms = 100 Hz

unsigned long previousSampleTime = 0;


// ============================================================
// FILTER CONFIGURATION
// ============================================================

const int FILTER_SIZE = 5;


// ============================================================
// FILTER STORAGE
// ============================================================

float axStorage[FILTER_SIZE];
float ayStorage[FILTER_SIZE];
float azStorage[FILTER_SIZE];

float gxStorage[FILTER_SIZE];
float gyStorage[FILTER_SIZE];
float gzStorage[FILTER_SIZE];


// ============================================================
// OBJECTS
// ============================================================

MovingAverage filterAx(axStorage, FILTER_SIZE);
MovingAverage filterAy(ayStorage, FILTER_SIZE);
MovingAverage filterAz(azStorage, FILTER_SIZE);

MovingAverage filterGx(gxStorage, FILTER_SIZE);
MovingAverage filterGy(gyStorage, FILTER_SIZE);
MovingAverage filterGz(gzStorage, FILTER_SIZE);

MPU6050 mpu;
Monitor monitor;
Adafruit_BME280 bme;


// ============================================================
// LED CONTROL
// ============================================================

void updateLEDs()
{
    SystemState state = monitor.getState();

    // Turn everything OFF first
    digitalWrite(LED_NORMAL, LOW);
    digitalWrite(LED_WARNING, LOW);
    digitalWrite(LED_ALARM, LOW);


    switch (state)
    {
        case NORMAL:

            digitalWrite(LED_NORMAL, HIGH);

            break;


        case WARNING:

            digitalWrite(LED_WARNING, HIGH);

            break;


        case ALARM:

            digitalWrite(LED_ALARM, HIGH);

            break;


        case FAULT:

            // Blink alarm LED every 250 ms
            if ((millis() / 250) % 2 == 0)
            {
                digitalWrite(LED_ALARM, HIGH);
            }

            break;
    }
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(500);


    // --------------------------------------------------------
    // I2C INITIALIZATION
    // --------------------------------------------------------

    Wire.begin(SDA_PIN, SCL_PIN);


    // --------------------------------------------------------
    // LED INITIALIZATION
    // --------------------------------------------------------

    pinMode(LED_NORMAL, OUTPUT);
    pinMode(LED_WARNING, OUTPUT);
    pinMode(LED_ALARM, OUTPUT);

    digitalWrite(LED_NORMAL, LOW);
    digitalWrite(LED_WARNING, LOW);
    digitalWrite(LED_ALARM, LOW);


    // --------------------------------------------------------
    // STARTUP MESSAGE
    // --------------------------------------------------------

    Serial.println();
    Serial.println("================================");
    Serial.println("ESP32 CONDITION MONITOR");
    Serial.println("MPU6050 + BME280");
    Serial.println("================================");


    // --------------------------------------------------------
    // MPU6050 INITIALIZATION
    // --------------------------------------------------------

    Serial.println("Starting MPU6050...");

    if (!mpu.begin())
    {
        Serial.println("MPU6050 ERROR");

        monitor.setMPUHealth(false);

        updateLEDs();

        // Stop normal operation
        while (true)
        {
            updateLEDs();
            delay(50);
        }
    }

    Serial.println("MPU6050 OK");

    monitor.setMPUHealth(true);


    // --------------------------------------------------------
    // BME280 INITIALIZATION
    // --------------------------------------------------------

    Serial.println("Starting BME280...");

    if (!bme.begin(BME280_ADDRESS))
    {
        Serial.println("BME280 ERROR");

        monitor.setBMEHealth(false);

        updateLEDs();

        // Stop normal operation
        while (true)
        {
            updateLEDs();
            delay(50);
        }
    }

    Serial.println("BME280 OK");

    monitor.setBMEHealth(true);


    // --------------------------------------------------------
    // SYSTEM READY
    // --------------------------------------------------------

    Serial.println();
    Serial.println("SYSTEM READY");
    Serial.println("================================");

    updateLEDs();
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop()
{
    unsigned long currentTime = millis();


    // ========================================================
    // 100 Hz SAMPLE TIMER
    // ========================================================

    if (currentTime - previousSampleTime < SAMPLE_INTERVAL)
    {
        return;
    }

    previousSampleTime = currentTime;


    // ========================================================
    // MPU6050 RAW DATA VARIABLES
    // ========================================================

    int16_t axRaw;
    int16_t ayRaw;
    int16_t azRaw;

    int16_t temperatureRaw;

    int16_t gxRaw;
    int16_t gyRaw;
    int16_t gzRaw;


    // ========================================================
    // READ MPU6050
    // ========================================================

    if (!mpu.read(
            axRaw,
            ayRaw,
            azRaw,
            temperatureRaw,
            gxRaw,
            gyRaw,
            gzRaw))
    {
        Serial.println("MPU6050 READ ERROR");

        monitor.setMPUHealth(false);

        updateLEDs();

        return;
    }


    // MPU communication successful
    monitor.setMPUHealth(true);


    // ========================================================
    // CONVERT MPU6050 DATA
    // ========================================================

    float ax = mpu.accelToG(axRaw);
    float ay = mpu.accelToG(ayRaw);
    float az = mpu.accelToG(azRaw);

    float gx = mpu.gyroToDegPerSec(gxRaw);
    float gy = mpu.gyroToDegPerSec(gyRaw);
    float gz = mpu.gyroToDegPerSec(gzRaw);

    float mpuTemperature =
        mpu.temperatureToC(temperatureRaw);


    // ========================================================
    // APPLY MOVING AVERAGE FILTER
    // ========================================================

    float filteredAx = filterAx.update(ax);
    float filteredAy = filterAy.update(ay);
    float filteredAz = filterAz.update(az);

    float filteredGx = filterGx.update(gx);
    float filteredGy = filterGy.update(gy);
    float filteredGz = filterGz.update(gz);


    // ========================================================
    // ACCELERATION MAGNITUDE
    // ========================================================

    float accelerationMagnitude =
        sqrt(
            filteredAx * filteredAx +
            filteredAy * filteredAy +
            filteredAz * filteredAz
        );


    // ========================================================
    // DYNAMIC ACCELERATION
    // Remove approximately 1 g of gravity
    // ========================================================

    float dynamicAcceleration =
        fabs(accelerationMagnitude - 1.0);


    // ========================================================
    // GYROSCOPE MAGNITUDE
    // ========================================================

    float gyroMagnitude =
        sqrt(
            filteredGx * filteredGx +
            filteredGy * filteredGy +
            filteredGz * filteredGz
        );


    // ========================================================
    // READ BME280
    // ========================================================

    float environmentalTemperature =
        bme.readTemperature();

    float pressure =
        bme.readPressure() / 100.0F;

    float humidity =
        bme.readHumidity();


    // ========================================================
    // VALIDATE BME280 DATA
    // ========================================================

    bool bmeValid =
        !isnan(environmentalTemperature) &&
        !isnan(humidity) &&
        !isnan(pressure);


    if (!bmeValid)
    {
        Serial.println("BME280 INVALID DATA");

        monitor.setBMEHealth(false);

        updateLEDs();

        return;
    }


    // BME communication/data successful
    monitor.setBMEHealth(true);


    // ========================================================
    // UPDATE MONITORING STATES
    // ========================================================

    monitor.update(
        dynamicAcceleration,
        gyroMagnitude,
        currentTime
    );


    monitor.updateBME(
        environmentalTemperature,
        humidity,
        pressure,
        currentTime
    );


    // ========================================================
    // UPDATE LEDs
    // ========================================================

    updateLEDs();


    // ========================================================
    // SERIAL MONITOR
    // ========================================================

    Serial.print("ACC: ");

    Serial.print(filteredAx, 2);
    Serial.print(", ");

    Serial.print(filteredAy, 2);
    Serial.print(", ");

    Serial.print(filteredAz, 2);

    Serial.print(" g");


    Serial.print(" | DYNAMIC: ");

    Serial.print(dynamicAcceleration, 3);

    Serial.print(" g");


    Serial.print(" | GYRO: ");

    Serial.print(filteredGx, 1);
    Serial.print(", ");

    Serial.print(filteredGy, 1);
    Serial.print(", ");

    Serial.print(filteredGz, 1);

    Serial.print(" deg/s");


    Serial.print(" | MPU TEMP: ");

    Serial.print(mpuTemperature, 2);

    Serial.print(" C");


    Serial.print(" | ENV TEMP: ");

    Serial.print(environmentalTemperature, 2);

    Serial.print(" C");


    Serial.print(" | HUMIDITY: ");

    Serial.print(humidity, 1);

    Serial.print(" %");


    Serial.print(" | PRESSURE: ");

    Serial.print(pressure, 2);

    Serial.print(" hPa");


    Serial.print(" | MPU STATE: ");

    Serial.print(monitor.getMPUStateName());


    Serial.print(" | BME STATE: ");

    Serial.print(monitor.getBMEStateName());


    Serial.print(" | SYSTEM STATE: ");

    Serial.println(monitor.getStateName());
}