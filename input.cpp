#include "input.h"
#include "src/io.h"
#include "src/debounceClass.h"

Debounce detector( detectorPin ) ;
Debounce greenButton( greenButtonPin ) ;
Debounce yellowButton( yellowButtonPin ) ;
Debounce redButton( redButtonPin ) ;
Debounce receiver( Rx ) ;
Debounce direction( directionPin ) ;


uint8 detectorState ;
uint8 greenButtonState ;
uint8 yellowButtonState ;
uint8 redButtonState ;
uint8 receiveState ;
uint8 directionState ;
uint8 buttonState ;
uint8 override ;

Signals signal ;

uint8 rxFreq = off ;


void readDipSwitches()
{
    signal.type = ( digitalRead( dip2 ) << 1 ) | digitalRead( dip1 ) ; // two, three or four tone
    signal.locked = digitalRead( dip3 ); 
    
}



void debounceDetector() // TESTED SUCCESFULLY
{
    static uint8 counter = 10 ;

    if( detectorState == LOW )
    {
        counter = 10 ;
        signal.track = OCCUPIED ;
    }

    REPEAT_MS( 100 )
    {
        if( counter > 0 )
        {
            if( -- counter == 0 )
            {
                signal.track = FREE ;
            }
        }
    } END_REPEAT
}


void debounceInputs() // TESTED SUCCESFULLY
{
    REPEAT_MS( 1 )
    {
        receiver.debounce() ;
    } END_REPEAT

    REPEAT_MS( 50 )
    {
        detector.debounce();
        greenButton.debounce();
        yellowButton.debounce();
        redButton.debounce();
        direction.debounce();
    } END_REPEAT

    detectorState       = detector.getState() ;
    greenButtonState    = greenButton.getState() ;
    yellowButtonState   = yellowButton.getState() ;
    redButtonState      = redButton.getState() ;
    receiveState        = receiver.getState() ;
    directionState      = direction.getState() ;

    debounceDetector() ;
}

void readIncFreq()
{
    static uint32 prevTime ;

    if( receiveState == RISING || receiveState == FALLING )
    {
        rxFreq = millis() - prevTime ;
        prevTime = millis() ;
    
        if(      rxFreq > (  greenFreq - 10 ) && rxFreq > (  greenFreq + 10 ) ) rxFreq = green ; 
        else if( rxFreq > ( yellowFreq - 10 ) && rxFreq > ( yellowFreq + 10 ) ) rxFreq = yellow ;
        else if( rxFreq > (yellowFreq2 - 10 ) && rxFreq > (yellowFreq2 + 10 ) ) rxFreq = yellow2 ;
        else if( rxFreq > (    redFreq - 10 ) && rxFreq > (    redFreq + 10 ) ) rxFreq = red ; 
        else                                                                    rxFreq = off ;
    }

    rxFreq = off ; // delete me
}

enum
{
    fallTimeOff,
    waitRising,
    setRed,
    setYellow,
    setYellow2,
} ;

uint8 fallTimeControl()
{
    static uint32 prevMillis ;
    static uint32 interval ;
    static uint8  fallTimeState = fallTimeOff ;

    switch( fallTimeState )
    {
    case fallTimeOff:
        if( signal.aspect == red )
        {
            prevMillis = millis() ;

            int sample =  analogRead( potPin ) ;
            interval = map( sample, 5, 1023, 3000, 60000 ) ;

// if fall time controll is disabled, the green or yellow button may set the aspect, yellow button must set override, green must not.
            if( sample < 5 ) 
            {
                if( greenButtonState  == FALLING )
                {
                    buttonState = green ;
                    return green ;
                }
                if( yellowButtonState == FALLING )
                {
                    override = true ;
                    buttonState = yellow ;
                    return yellow ;
                }
                return red;
            }
            fallTimeState = setRed ;
        }
        return off ;

// if fall time controll is enabled, the green or yellow button should override the aspect and terminate the fall time
    case setRed :

        if( greenButtonState  == FALLING )
        {
            fallTimeState = fallTimeOff ;
            buttonState = green ;
            return green ;
        }
        if( yellowButtonState == FALLING )
        {
            fallTimeState = fallTimeOff ;
            buttonState = yellow ;
            override = true ;
            return yellow ;
        }
        if( millis() - prevMillis >= interval )          // time has expired, we now take the next aspect
        {
            prevMillis = millis() ;

            if( signal.type ==   TWO_TONE ) fallTimeState = fallTimeOff ;         
            if( signal.type == THREE_TONE ) fallTimeState = setYellow ;         
            if( signal.type ==  FOUR_TONE ) fallTimeState = setYellow2 ;
        }
        return red ;

    case setYellow2 :
        if( greenButtonState  == FALLING )
        {
            fallTimeState = fallTimeOff ;
            buttonState = green ;
            return green ;
        }
        if( millis() - prevMillis >= interval )
        {
            fallTimeState = setYellow ;
        }
        return yellow2 ;

    case setYellow :
        if( greenButtonState  == FALLING )
        {
            fallTimeState = fallTimeOff ;
            buttonState = green ;
            return green ;
        }
        if( millis() - prevMillis >= interval )
        {
            fallTimeState = fallTimeOff ;
        }
        return yellow ; 
    }
}
