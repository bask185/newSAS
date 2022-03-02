#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"
#include "input.h"
#include "output.h"

const int incFrequency[/*tone*/ 3][/* color */ 5] = {
//  green, yellow, yellow2,     red   no signal <-- incoming frequency
  { green,  green,   green,   green,   off },// <-- two tone
  { green,  green,   green,  yellow,   off },// <-- three tone
  { green,  green,  yellow, yellow2,   off } // <-- four tone
} ;


void setup()
{
    initIO() ;
    debounceInputs() ;
    delay(100);
    debounceInputs() ;
    delay(100);
    debounceInputs() ;

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


void computeLogic() // TAKES ALL INPUT IN ACOUNT AND SET THE SIGNAL AND BRAKE MODUE ASPECTS ACCORDINGLY
{
//********************** DIRECTION LINE  **********************/    
    if( directionState == LOW )
    {
        override = false ;
        signal.aspect = red ;
        if( signal.locked ) { signal.brakeModule = green ; }
        else                { signal.brakeModule =   red ; } 
        return ;
    }
/**********************  DETECTOR **********************/
    if( signal.track == OCCUPIED )
    {
        override = false ;
        signal.aspect = red ;
        signal.brakeModule = red ;
        return ;
    }

//********************** BUTTONS ********************************/
    if( override == true )
    {
        signal.aspect = buttonState ;
        if( buttonState == red )    signal.brakeModule =   red ;
        else                        signal.brakeModule = green ;
        return ;
    }      

/**********************  ADJACENT SIGNALS   **********************/
    
    uint8 newState = incFrequency[ signal.type ][ rxFreq ] ;
    if( newState != off )                                                       // TEST ME
    {
        signal.aspect      = newState ;
        signal.brakeModule = newState ;
        return ;
    }

    /*
    if( signal.type == TWO_TONE )                                               // works fine
    {
        switch( rxFreq ) 
        {
            case  green :  signal.aspect =  green ; signal.brakeModule = green ; return ;
            case    red :  signal.aspect =  green ; signal.brakeModule = green ; return ;
            case    off : break ;
        }
    }
    else if( signal.type == THREE_TONE )
    {
        switch( rxFreq )
        { 
            case  green : signal.aspect =  green ; signal.brakeModule =  green ; return ;
            case yellow : signal.aspect =  green ; signal.brakeModule =  green ; return ;
            case    red : signal.aspect = yellow ; signal.brakeModule = yellow ; return ;
            case    off : break ;
        }
    }
    */

/**********************  FALL TIME CONTROL  **********************/

    switch( fallTimeControl() )
    {
        case   green : signal.aspect =   green ; signal.brakeModule =  green ; return ;
        case  yellow : signal.aspect =  yellow ; signal.brakeModule = yellow ; return ;
        case yellow2 : signal.aspect = yellow2 ; signal.brakeModule = yellow ; return ;
        case     red : signal.aspect =     red ; signal.brakeModule =    red ; return ;
        case     off : break ;
    }

    signal.aspect =  green ; signal.brakeModule =  green ;                                                 // if no input at all, show green aspect
}


void loop()
{
    debounceInputs() ;

    computeLogic() ;

    setLeds() ;

    controlSemaphore() ;

    setBrakeModule() ;

/*TODO
    // add EEPROM + config
    // sendTxSignals() ;
    // fall time does not seem to work
    */
}