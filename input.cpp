#include "input.h"
#include "src/io.h"
#include "src/debounceClass.h"
#include "src/stateMachineClass.h"

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
uint8 fallTimeOn ;

Signals signal ;

uint8 rxFreq = off ;


void readDipSwitches()
{
    signal.type = ( !digitalRead( dip1 ) << 1 ) | !digitalRead( dip2 ) ; // two, three or four tone
    signal.passBehind = digitalRead( dip3 ); 
    
}



void debounceDetector() // TESTED SUCCESFULLY
{
    static uint8 counter = 10 ;

    if( detectorState == LOW )
    {
        counter = 10 ;
        signal.track = OCCUPIED ;
        fallTimeOn = 1 ;
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

uint8 readButtons()
{
    if(  greenButtonState == FALLING ) return green ;
    if( yellowButtonState == FALLING ) return yellow ;
    if(    redButtonState == FALLING ) return red ;

    return off ;
}

void readIncFreq()
{
    if( receiveState == RISING || receiveState == FALLING )
    {
        static uint32 prevTime = 0 ;
        uint32 lastTime = millis() - prevTime ;

        prevTime = millis() ;
    
        if(      lastTime > (  greenFreq - 10 ) && lastTime < (  greenFreq + 10 ) ) rxFreq = green ; 
        else if( lastTime > ( yellowFreq - 10 ) && lastTime < ( yellowFreq + 10 ) ) rxFreq = yellow ;
        else if( lastTime > (yellow2Freq - 10 ) && lastTime < (yellow2Freq + 10 ) ) rxFreq = yellow2 ;
        else if( lastTime > (    redFreq - 10 ) && lastTime < (    redFreq + 10 ) ) rxFreq = red ; 
        else                                                                        rxFreq = off ;
    }
}

enum
{
    fallTimeOff,
    waitRising,
    setRed,
    setYellow,
    setYellow2,
} ;

void blink()
{
    digitalWrite( greenLedPin, HIGH ) ;
    delay(200);
    digitalWrite( greenLedPin,  LOW ) ;
    delay(200);
}

#define TIME_EXPIRED millis() - prevMillis >= interval
#define LOAD_TIMER   prevMillis = millis()

uint8 fallTimeControl()
{
    static uint32 prevMillis ;
    static uint32 interval ;
    static uint16 sample ;
    static uint8  fallTimeState = fallTimeOff ;
    static bool   entryState = true ;

    REPEAT_MS( 100 )                                                            // update adc and interval sample every 100ms
    {
        sample   = analogRead( potPin ) ;
        interval = map( sample, 5, 1023, 500, 10000 ) ; // MAKE ME 60K
    } END_REPEAT


    switch( fallTimeState )
    {
    case fallTimeOff:
        if( entryState )
        {   entryState = false ;

            return green ;
        }

        if( rxFreq != off || sample < 5 ) return off ;                          // if receiving a frequency, don't run fall time controll   

        if( signal.track == FREE && fallTimeOn == 1 )
        {
            LOAD_TIMER ;
            fallTimeOn    = 0 ;
            entryState    = true ;                                              // upon entering this state, the signal must become green again
            fallTimeState = setRed ;
        }
        return off ;

    case setRed :
        if( TIME_EXPIRED )                                                      // keep return red red until the time expired
        {   LOAD_TIMER ;

            if( signal.type ==  FOUR_TONE ) fallTimeState = setYellow2  ;
            if( signal.type == THREE_TONE ) fallTimeState = setYellow   ;         
            if( signal.type ==   TWO_TONE ) fallTimeState = fallTimeOff ;
        }
        return red ;

    case setYellow2 :
        if( TIME_EXPIRED )
        {   LOAD_TIMER ;
            
            fallTimeState = setYellow ;
        }
        return yellow2 ;

    case setYellow :
        if( TIME_EXPIRED )
        {
            fallTimeState = fallTimeOff ; 
        }
        return yellow ; 
    }
}
