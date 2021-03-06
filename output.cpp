#include "output.h"
#include "input.h"
#include "src/io.h"


uint8 mainMinPos ;
uint8 mainGreen ;
uint8 greenPwm ;
uint8 yellowPwm ;
uint8 yellowPwm2 ;
uint8 redPwm ;

ServoSweep semaphore( servoPin_, 90, 135, 1, 20  ) ;
ServoSweep dist(      255,       90, 135, 1, 20  ) ;

#define setSignals(a,b,c,d) \
{ \
    digitalWrite( greenLedPin, a ) ; digitalWrite( yellowLedPin,  b ) ; digitalWrite( yellowLedPin2,  c ) ; digitalWrite( redLedPin,  d ) ; \
}
void setLeds()
{
    if( signal.aspect ==   green ) { setSignals( HIGH,  LOW,  LOW,  LOW ) ; } // must be fading in the future
    if( signal.aspect ==  yellow ) { setSignals(  LOW, HIGH,  LOW,  LOW ) ; }
    if( signal.aspect == yellow2 ) { setSignals(  LOW, HIGH, HIGH,  LOW ) ; }
    if( signal.aspect ==     red ) { setSignals(  LOW,  LOW,  LOW, HIGH ) ; }
}
#undef setSignals


void controlmain()
{
    semaphore.sweep() ;
    dist.sweep() ;

    static uint8 prevAspect = 0x99 ;                                  // magic number
    if( prevAspect != signal.aspect )
    {   prevAspect  = signal.aspect ;

        if( signal.aspect == red || signal.aspect == yellow || signal.aspect == yellow2 ) semaphore.setState( 1 ) ;
        if( signal.aspect == green )                          semaphore.setState( 0 ) ;
    }
}

void setBrakeModule()
{
    /* NB there should be some delay in the order of ~10 seconds before the break mopule is set.
    * in the event of a shut off relay, there should be some time to let the train pass this section
    */

    // switch( signal.brakeModule )
    // {
    //     case  green : digitalWrite( relayPin_,  LOW ) ; digitalWrite( slowSpeed,  LOW ) ; break ;
    //     case yellow : digitalWrite( relayPin_, HIGH ) ; digitalWrite( slowSpeed, HIGH ) ; break ;
    //     case    red : digitalWrite( relayPin_, HIGH ) ; digitalWrite( slowSpeed,  LOW ) ; break ;
    // }

    REPEAT_MS( greenFreq )
    {
        digitalWrite( relayPin_, !digitalRead( relayPin_) ) ;

    } END_REPEAT  

    REPEAT_MS( redFreq )
    {
        digitalWrite( slowSpeed, !digitalRead( slowSpeed) ) ;      

    } END_REPEAT
}

#define setFreq(x) case x: freq = x##Freq ; break ;
void sendTxSignals()
{
    uint8 freq ;
    switch( signal.aspect )
    {
        setFreq(   green ) ;
        setFreq(  yellow ) ;
        setFreq( yellow2 ) ;
        setFreq(     red ) ;
    }
    
    REPEAT_MS( freq )
    {
        digitalWrite( slowSpeed, !digitalRead( slowSpeed) ) ;      

    } END_REPEAT
}
#undef getFreq