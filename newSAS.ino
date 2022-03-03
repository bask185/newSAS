#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"
#include "input.h"
#include "output.h"
#include "teachin.h"

const int incFrequency[/*tone*/ 3][/* color */ 5] = {
//  green, yellow, yellow2,     red   no signal <-- incoming frequency
  {   off,  green,   green,   green,   off },// <-- two tone
  {   off,  green,   green,  yellow,   off },// <-- three tone
  {   off,  green,  yellow, yellow2,   off } // <-- four tone
//  N.B. in order to work with short detectors, a green aspect of up-line signal
//       is ignored. This will make sure that the signal does not become green
//       as soon as the detector falls off
} ;


void setup()
{
    initIO() ;
    debounceInputs() ;
    delay(100);
    debounceInputs() ;
    delay(100);
    debounceInputs() ;

    teachInInit() ;

    signal.type = THREE_TONE ;
    signal.track = FREE ;
    signal.aspect = green ;
}



/*
things TODO for version 2.
-  the pre signal must work differently if it is attached to a main signal down line.
   I can use a dip setting for that. (attached/detached pre signal)

- similarly. The led setting for a german pre signal may be different in nature. I could use the fourth signal type.
  the block signal must use green and red, but the pre signal should use y1 and y2 and may contain either expect green of expect red
  this is only needed when pre signal is attached to main signal

*/


void controlSignalAspect() // TAKES ALL INPUT IN ACOUNT AND SET THE SIGNAL AND BRAKE MODUE ASPECTS ACCORDINGLY
{
    uint8_t newState = off ;
//********************** DIRECTION LINE  **********************/    
    if( directionState == LOW )
    {
        signal.aspect = red ;
        if( signal.passBehind ) { signal.brakeModule = green ; }
        else                    { signal.brakeModule =   red ; } 
        return ;
    }
/**********************  DETECTOR *****************************/
    if( signal.track == OCCUPIED )
    {
        signal.aspect = red ;
        signal.brakeModule = red ;
        return ;
    }

/**********************  ADJACENT SIGNALS   ********************/

    newState = incFrequency[ signal.type ][ rxFreq ] ;
    if( newState != off )
    {
        signal.aspect      = newState ;
        signal.brakeModule = newState ;
        return ;
    }

/**********************  FALL TIME CONTROL  ********************/
    newState = fallTimeControl() ;
    if( newState != off )
    {
        signal.aspect      = newState ;
        signal.brakeModule = newState ;
        return ;
    }

//********************** BUTTONS ********************************/
    newState = readButtons() ;
    if( newState != off )
    {
        signal.aspect      = newState ;
        signal.brakeModule = newState ;
        return ;
    }
}


void loop()
{
    readDipSwitches() ;
    debounceInputs() ;
    readIncFreq() ;

    controlSignalAspect() ;

    if( teachIn() == waitButtonPress )  // if config mode is in this state,     control all outputs
    {
        setLeds() ;
        controlmain() ;
        setBrakeModule() ;
        //sendTxSignals() ;
    }
}