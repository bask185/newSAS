#include <Arduino.h>
#include "src/macros.h"
#include "src/ServoSweep.h"

extern ServoSweep semaphore ;
extern ServoSweep dist ;

extern void  setLeds() ;
extern void  controlmain() ;
extern void  setBrakeModule() ; 
extern void  sendTxSignals() ;
extern uint8 readButtons() ;
