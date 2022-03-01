#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"
#include "input.h"
#include "output.h"

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




void computeLogic() // IN ORDER OF IMPORTANCE
{
//********************** BUTTONS ********************************/
     static uint8 buttonState, override = false ;

    if( greenButtonState  == FALLING ) { 
        if( override == true ) override = false ;
        else
        {
            buttonState =  green ; 
            override = true ;
        }
    }
    if( yellowButtonState == FALLING ) { buttonState = yellow ; override =  true ; } 
    if( redButtonState    == FALLING ) { buttonState =    red ; override =  true ; }

    if( override == true )
    {
        signal.aspect = buttonState ;
        if( buttonState == red )    signal.brakeModule =   red ;
        else                        signal.brakeModule = green ;
        return ;
    }      

//********************** DIRECTION LINE **********************/    
    if( directionState == LOW )
    {
        signal.aspect = red ;
        if( signal.passBehind ) { signal.brakeModule = green ; }
        else                    { signal.brakeModule =   red ; } 
        return ;
    }
/**********************  DETECTOR **********************/
    if( signal.track == OCCUPIED )
    {
        signal.aspect = red ;
        signal.brakeModule = red ;
        return ;
    }

/**********************  ADJACENT SIGNALS **********************/
    
    if( signal.type == TWO_TONE ) 
    {
        switch( rxFreq ) 
        {
            case  green :  signal.aspect =  green ; signal.brakeModule =  green ; return ;
            case    red :  signal.aspect =  green ; signal.brakeModule =  green ; return ;
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

/**********************  FALL TIME CONTROL  **********************/

    switch( fallTimeControl() )
    {
        case yellow : signal.aspect = yellow ; signal.brakeModule = yellow ; return ;
        case    red : signal.aspect =    red ; signal.brakeModule =    red ; return ;
        case    off : break ;
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