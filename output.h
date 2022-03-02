#include <Arduino.h>
#include "src/macros.h"


extern uint8 servoPosMin ;
extern uint8 servoPosMax ;

extern void setLeds() ;
extern void controlSemaphore() ;
extern void initializeSemaphore() ;
extern void setBrakeModule() ; 