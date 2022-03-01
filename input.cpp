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

Signals signal ;

uint8 rxFreq = off ;



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
    
        if(      rxFreq > ( greenFreq - 10 ) && rxFreq > ( greenFreq + 10 ) ) rxFreq = green ; 
        else if( rxFreq > (yellowFreq - 10 ) && rxFreq > (yellowFreq + 10 ) ) rxFreq = yellow ;
        else if( rxFreq > (   redFreq - 10 ) && rxFreq > (   redFreq + 10 ) ) rxFreq = red ; 
        else                                                                  rxFreq = off ;
    }

    rxFreq = off ; // delete me
}

enum {
    idle,
    waitRising,
    setRed,
    setYellow,
} ;

uint8 fallTimeControl()
{
    static uint32 prevMillis ;
    static uint32 interval ;
    static uint8  state = idle ;

    switch( state )
    {
    case idle:
        if( signal.aspect == red )
        {
            prevMillis = millis() ;

            int sample =  analogRead( potPin ) ;
            interval = map( sample, 0, 1023, 3000, 60000 ) ;

             
            if( sample < 10 ) return red;

            state = setRed ;
        }
        return off ;

    case setRed :
        if( millis() - prevMillis >= interval )
        {
            if( signal.type == THREE_TONE )
            {
                state = setYellow ;
                prevMillis = millis() ;
            }
            else
            {
                state = idle ;
            }
        }
        return red ;

    case setYellow :
        if( millis() - prevMillis >= interval )
        {
            state = idle ;
        }
        return yellow ; 
    }
}
