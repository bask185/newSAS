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



/* How should the buttons work, and what must they override or not?

Occupied track or wrong direction must override the button states unconditionally
if fall time controll is disabled, the green or yellow button may set the signal, green must not set override, yellow button must set override
if fall time controll is enabled, the green or yellow button should override the aspect and terminate the fall time
if no detection is used at all, the signal should simply follow the 3 button states.

things TODO
fall time controll, does not yet have 4 tone
the 4th tone should be added to signal state.
the 4th tone hould be added to falltime controll

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