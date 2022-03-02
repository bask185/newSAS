#include <Arduino.h>
#include "src/macros.h"

enum teachInStates {
    teachInIDLE,
    adjustGreenBrightness,
    adjustYellowBrightness,
    adjustRedBrightness,
    setServoRed,
    setServoGreen,
    waitButtonPress
} ;


extern uint8 teachIn(void) ; 
extern void teachInInit() ;
