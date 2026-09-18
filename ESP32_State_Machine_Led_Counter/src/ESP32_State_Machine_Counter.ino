// ============================================================
//              ESP32 4-BIT COUNTER + STATE MACHINE
// ============================================================
//
// BUTTON:
//     GPIO 21
//
// LEDS:
//     Bit 0 -> GPIO 25  RED 
//     Bit 1 -> GPIO 33  GREEN
//     Bit 2 -> GPIO 32  YELLOW
//     Bit 3 -> GPIO 26  BLUE
//
// BUTTON WIRING:
//
//     GPIO 21
//         |
//       BUTTON
//         |
//        GND
//
// INPUT_PULLUP:
//
//     Released = HIGH
//     Pressed  = LOW
//
// BEHAVIOR:
//
//     Short press -> counter + 1
//     Long press  -> counter - 1
//     Not pressed -> wait
//
// COUNTER:
//
//     0 -> 1 -> 2 -> ... -> 15 -> 0
//
//     15 -> 14 -> ... -> 0 -> 15
//
// ============================================================


// ============================================================
//                       PIN DEFINITIONS
// ============================================================

const int BUTTON_PIN = 21;

const int LED_BIT0 = 25;
const int LED_BIT1 = 33;
const int LED_BIT2 = 32;
const int LED_BIT3 = 13;


// ============================================================
//                         TIMING
// ============================================================

const unsigned long DEBOUNCE_TIME   = 50;
const unsigned long LONG_PRESS_TIME = 1000;


// ============================================================
//                         COUNTER
// ============================================================

uint8_t counter = 0;


// ============================================================
//                       STATE MACHINE
// ============================================================

enum ButtonState
{
    NOT_PRESSED,
    PRESSED,
    SHORT_PRESS,
    LONG_PRESS
};

ButtonState currentState = NOT_PRESSED;


// ============================================================
//                     BUTTON VARIABLES
// ============================================================

int lastButtonReading = HIGH;

int stableButtonState = HIGH;

unsigned long lastDebounceTime = 0;

unsigned long buttonPressTime = 0;


// ============================================================
//                           SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);


    // -------------------- BUTTON --------------------

    pinMode(BUTTON_PIN, INPUT_PULLUP);


    // --------------------- LEDS ---------------------

    pinMode(LED_BIT0, OUTPUT);

    pinMode(LED_BIT1, OUTPUT);

    pinMode(LED_BIT2, OUTPUT);

    pinMode(LED_BIT3, OUTPUT);


    // ---------------- INITIAL DISPLAY ---------------

    displayCounter();


    Serial.println("4-BIT COUNTER");
    Serial.println("Short press = +1");
    Serial.println("Long press  = -1");
}


// ============================================================
//                            LOOP
// ============================================================

void loop()
{
    readButton();

    updateStateMachine();
}


// ============================================================
//                        READ BUTTON
// ============================================================

void readButton()
{
    int reading = digitalRead(BUTTON_PIN);


    // --------------------------------------------------------
    // Check if raw button signal changed
    // --------------------------------------------------------

    if (reading != lastButtonReading)
    {
        lastDebounceTime = millis();
    }


    // --------------------------------------------------------
    // Check if signal has been stable
    // --------------------------------------------------------

    if ((millis() - lastDebounceTime) > DEBOUNCE_TIME)
    {
        if (reading != stableButtonState)
        {
            stableButtonState = reading;


            // ------------------------------------------------
            // BUTTON PRESSED
            // ------------------------------------------------

            if (stableButtonState == LOW)
            {
                buttonPressTime = millis();

                currentState = PRESSED;
            }


            // ------------------------------------------------
            // BUTTON RELEASED
            // ------------------------------------------------

            else
            {
                unsigned long pressDuration;

                pressDuration = millis() - buttonPressTime;


                // --------------------------------------------
                // SHORT PRESS
                // --------------------------------------------

                if (pressDuration < LONG_PRESS_TIME)
                {
                    currentState = SHORT_PRESS;
                }


                // --------------------------------------------
                // LONG PRESS
                // --------------------------------------------

                else
                {
                    currentState = LONG_PRESS;
                }
            }
        }
    }


    lastButtonReading = reading;
}


// ============================================================
//                    UPDATE STATE MACHINE
// ============================================================

void updateStateMachine()
{
    switch (currentState)
    {
        // ----------------------------------------------------
        //                    NOT PRESSED
        // ----------------------------------------------------

        case NOT_PRESSED:

            break;


        // ----------------------------------------------------
        //                       PRESSED
        // ----------------------------------------------------

        case PRESSED:

            break;


        // ----------------------------------------------------
        //                    SHORT PRESS
        // ----------------------------------------------------

        case SHORT_PRESS:

            counter++;

            counter = counter & 0x0F;

            displayCounter();

            Serial.print("SHORT PRESS  -> Counter = ");

            Serial.println(counter);

            currentState = NOT_PRESSED;

            break;


        // ----------------------------------------------------
        //                     LONG PRESS
        // ----------------------------------------------------

        case LONG_PRESS:

            if (counter == 0)
            {
                counter = 15;
            }
            else
            {
                counter--;
            }


            displayCounter();

            Serial.print("LONG PRESS   -> Counter = ");

            Serial.println(counter);

            currentState = NOT_PRESSED;

            break;
    }
}


// ============================================================
//                    DISPLAY 4-BIT COUNTER
// ============================================================
//
// COUNTER:
//
//     0b0000
//     ││││
//     │││└── Bit 0
//     ││└─── Bit 1
//     │└──── Bit 2
//     └───── Bit 3
//
// Each bit controls one LED.
//
// ============================================================

void displayCounter()
{
    digitalWrite(
        LED_BIT0,
        (counter >> 0) & 0x01
    );


    digitalWrite(
        LED_BIT1,
        (counter >> 1) & 0x01
    );


    digitalWrite(
        LED_BIT2,
        (counter >> 2) & 0x01
    );


    digitalWrite(
        LED_BIT3,
        (counter >> 3) & 0x01
    );
}
