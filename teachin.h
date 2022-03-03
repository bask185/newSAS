#include <Arduino.h>
#include "src/macros.h"

enum teachInStates {
    teachInIDLE,
    adjustGreenBrightness,
    adjustYellowBrightness,
    adjustYellow2Brightness,
    adjustRedBrightness,
    setMainRed,
    setMainGreen,
    setDistRed,
    setDistGreen,
    waitButtonPress
} ;

typedef struct
{
    uint8  mainGreen ;
    uint8    mainRed ;
    uint8  distGreen ;
    uint8    distRed ;
    uint8   greenPwm ;
    uint8  yellowPwm ;
    uint8 yellow2Pwm ;
    uint8     redPwm ;
    uint8        def ;
} Settings ;

extern Settings setting ;


extern uint8 teachIn(void) ; 
extern void teachInInit() ;
