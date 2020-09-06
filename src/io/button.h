#include <Arduino.h>

// this file contains structures for easily reading buttons. provides advanced reading: tapped, held etc...

const int BUTTON_LONG_PRESS_TIME = 500;
const int BUTTON_TAP_TIME = 300;
const byte BUTTON_AMT = 4; // buttons outputting midi notes

struct Button
{
    bool input = false;   // input buffer
    bool didHold = false; // this stores if button was long pressed
    long t0 = 0;

    // user vars. read these to use button.
    bool fell = false;    // read this to see if fell
    bool rose = false;    // read this to see if rose
    bool pressed = false; // read this to see if buttton is pressed at the time
    bool tapped = false;  // read this to see if tapped
    bool held = false;    // read this to see if held
};

Button minButton;
Button maxButton;
Button modeButton;
Button controllerButton[BUTTON_AMT];

void updateButton(Button &myButton, bool inputVal, long tFrame) // inputVal false = button pressed down
{

    // set vars to default state
    myButton.tapped = false;
    myButton.held = false;
    myButton.fell = false;
    myButton.rose = false;

    myButton.input = inputVal;                // read input
    if (!myButton.input && !myButton.pressed) // new button press
    {
        myButton.didHold = false; // reset temp variable
        myButton.pressed = true;
        myButton.fell = true;
        myButton.t0 = tFrame;
    }

    if (myButton.input && myButton.pressed) // button released
    {
        myButton.pressed = false;
        myButton.rose = true;

        if (tFrame - myButton.t0 < BUTTON_TAP_TIME)
        {
            myButton.tapped = true;
        }
    }

    if (!myButton.didHold && myButton.pressed && (tFrame - myButton.t0 > BUTTON_LONG_PRESS_TIME)) // if button was held long enough
    {
        myButton.held = true;
        myButton.didHold = true;
    }
}

void updateButtons(long tFrame)
{
    updateButton(minButton, shiftInReadBit(4), tFrame);
    updateButton(maxButton, shiftInReadBit(5), tFrame);
    updateButton(modeButton, shiftInReadBit(6), tFrame);
    updateButton(controllerButton[0], shiftInReadBit(0), tFrame);
    updateButton(controllerButton[1], shiftInReadBit(1), tFrame);
    updateButton(controllerButton[2], shiftInReadBit(2), tFrame);
    updateButton(controllerButton[3], shiftInReadBit(3), tFrame);

    // debugging stuff

    // Serial.print("min button ");
    // Serial.println(minButton.pressed);
    // if(minButton.pressed)
    // Serial.println("minButton.pressed");
    // if(maxButton.pressed)
    // Serial.println("maxButton.pressed");
    // if(modeButton.pressed)
    // Serial.println("modeButton.pressed");

    // for (byte i = 0; i < 4; i++)
    // {
    //     if (controllerButton[i].fell)
    //     {

    //         oledPrint("Button " + String(i) + " pressed", 0, 0, 1);
    //         delay(500);
    //     }
    // }
}