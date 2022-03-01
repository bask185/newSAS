#include "output.h"
#include "input.h"
#include "src/io.h"
#include "src/ServoSweep.h"

ServoSweep semaphore( servoPin_, 90, 135, 20, 1  ) ;

void setLeds()
{
    if( signal.aspect ==  green ) { digitalWrite( greenLedPin, HIGH ) ; digitalWrite( yellowLedPin,  LOW ) ; digitalWrite( redLedPin,  LOW ) ; } // must be fading in the future
    if( signal.aspect == yellow ) { digitalWrite( greenLedPin,  LOW ) ; digitalWrite( yellowLedPin, HIGH ) ; digitalWrite( redLedPin,  LOW ) ; }
    if( signal.aspect ==    red ) { digitalWrite( greenLedPin,  LOW ) ; digitalWrite( yellowLedPin,  LOW ) ; digitalWrite( redLedPin, HIGH ) ; }
}

void initializeSemaphore()
{
    semaphore.begin() ;
    semaphore.setMin( 90) ;     // TODO get this from EEPROM
    semaphore.setMax( 135 ) ;
}

void controlSemaphore()
{
    semaphore.sweep() ;

    static uint8 prevAspect = 0x99 ;                                  // magic number
    if( prevAspect != signal.aspect )
    {   prevAspect  = signal.aspect ;

        if( signal.aspect == red || signal.aspect == yellow ) semaphore.setState( 1 ) ;
        if( signal.aspect == green )                          semaphore.setState( 0 ) ;
    }
}

void setBrakeModule()
{
    switch( signal.aspect )
    {
        case  green : digitalWrite( relayPin_,  LOW ) ; digitalWrite( slowSpeed,  LOW ) ; break ;
        case yellow : digitalWrite( relayPin_, HIGH ) ; digitalWrite( slowSpeed, HIGH ) ; break ;
        case    red : digitalWrite( relayPin_, HIGH ) ; digitalWrite( slowSpeed,  LOW ) ; break ;
    }
}