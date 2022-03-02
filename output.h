#include <Arduino.h>
#include "src/macros.h"
#include "src/ServoSweep.h"

extern ServoSweep semaphore ;

extern uint8 servoPosMin ;
extern uint8 servoPosMax ;
extern uint8 greenPwm ;
extern uint8 yellowPwm ;
extern uint8 yellowPwm2 ;
extern uint8 redPwm ;

extern void setLeds() ;
extern void controlSemaphore() ;
extern void initializeSemaphore() ;
extern void setBrakeModule() ; 
extern void sendTxSignals() ;
