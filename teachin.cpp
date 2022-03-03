// HEADER FILES
#include "teachIn.h"
#include <EEPROM.h>
#include "src/io.h"
#include "output.h"
#include "input.h"

#include "src/stateMachineClass.h"

#define entryState  if( sm.entryState() )
#define onState     if( sm.onState() )
#define exitState   if( sm.exitState() == 0 ) return 0 ; \
                    else

static StateMachine sm ;
Settings setting ;


enum EEaddresses
{
    PWM_GREEN_ADDR = 0x00,
    PWM_YELLOW_ADDR,
    PWM_YELLOW_ADDR2,
    PWM_RED_ADDR,
    GREEN_MAIN_POS,
    RED_MAIN_POS,
    GREEN_DIST_POS,
    RED_DIST_POS,
    INIT_ADDR,
} ; 



uint16 adcSample ;
uint8  pwm ;
const int magicMike = 0xCC ;
// VARIABLES

// FUNCTIONS
extern void teachInInit(void)
{ 
    sm.setState( waitButtonPress ) ;

    EEPROM.get( PWM_GREEN_ADDR, setting ) ;

    if( setting.def != magicMike )              // if this byte in eeprom is wrong, program runs first time and must first load defaults
    {
        setting.mainGreen   = 135 ;
        setting.mainRed     =  90 ;
        setting.distGreen   = 135 ;
        setting.distRed     =  90 ;
        setting.greenPwm    = 255 ;
        setting.yellowPwm   = 255 ;
        setting.yellow2Pwm  = 255 ;
        setting.redPwm      = 255 ;
        setting.def         = magicMike ;

        EEPROM.put( PWM_GREEN_ADDR, setting ) ;
        Serial.println("DEFAULTS LOADED") ;

        semaphore.begin() ;
        semaphore.setMin( setting.mainGreen) ;   
        semaphore.setMax( setting.mainRed ) ;

        dist.begin() ;
        dist.setMin( setting.distGreen) ;   
        dist.setMax( setting.distRed ) ;
    }
}

// STATE FUNCTIONS

StateFunction( waitButtonPress )// just wait on the first button press
{ 
    entryState 
    {
        Serial.println("run mode") ;
    }
    onState 
    {
        if( adcSample <= 3 ) sm.exit() ;        
    }
    exitState 
    {
        Serial.println("entering config mode") ;
        return true;
    }
}

StateFunction( adjustGreenBrightness )
{
    entryState
    {
        digitalWrite( greenLedPin,   LOW ) ;
        digitalWrite( yellowLedPin,  LOW ) ;
        digitalWrite( yellowLedPin2, LOW ) ;
        digitalWrite( redLedPin,     LOW ) ;
        sm.setTimeout( 30000 ) ;
    }
    onState
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit(); // if timeout or button press occurs, exit
        else setting.greenPwm = map( adcSample, 0, 1023, 0, 255 ) ;
        
        analogWrite( greenLedPin, setting.greenPwm ) ;
    }
    exitState
    {
        EEPROM.write( PWM_GREEN_ADDR, setting.greenPwm ) ;
        digitalWrite( greenLedPin, LOW ) ;
        return true;
    }
}

StateFunction( adjustYellowBrightness )
{
    entryState 
    {
        sm.setTimeout( 30000 ) ;
    }
    onState 
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();
        else  pwm = map ( adcSample, 0, 1023, 0, 255 ) ;

        analogWrite(yellowLedPin, pwm);

    }
    exitState 
    {
        digitalWrite(yellowLedPin, LOW );
        EEPROM.write( PWM_YELLOW_ADDR, pwm  ) ;
        return true;
    }
}

StateFunction( adjustYellow2Brightness )
{
    entryState 
    {
        sm.setTimeout( 30000 ) ;
    }
    onState 
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();
        else setting.yellow2Pwm = map ( adcSample, 0, 1023, 0, 255 ) ;

        analogWrite( yellowLedPin2, setting.yellow2Pwm);
    }
    exitState 
    {
        digitalWrite( yellowLedPin2, LOW );
        EEPROM.write( PWM_YELLOW_ADDR, setting.yellow2Pwm  ) ;
        return true;
    }
}


StateFunction( adjustRedBrightness )
{
    entryState 
    {
        sm.setTimeout( 30000 ) ;
    }
    onState
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();
        else setting.redPwm = map ( adcSample, 0, 1023, 0, 255 ) ;

        analogWrite( redLedPin, setting.redPwm ) ;
    }
    exitState
    {
        digitalWrite( redLedPin, LOW ) ;
        EEPROM.write( PWM_RED_ADDR, setting.redPwm ) ;
        return true;
    }
}



StateFunction( setMainGreen )
{
    entryState
    {
        sm.setTimeout( 30000 ) ;
    }
    onState
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();

        else
        {  
            setting.mainGreen = map( adcSample, 0, 1023, 0, 180) ;
            semaphore.write( setting.mainGreen ) ;
        }
    }
    exitState
    {
        EEPROM.write( GREEN_MAIN_POS, setting.mainGreen ) ;
        return true;
    }
}



StateFunction( setMainRed )
{
    entryState
    { 
        sm.setTimeout( 30000 ) ;
    }

    onState
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();

        else
        {   
            setting.mainRed = map( adcSample, 0, 1023, 0, 180) ;
            semaphore.write( setting.mainRed ) ;
        }
    }

    exitState
    {
        EEPROM.write( RED_MAIN_POS, setting.mainRed ) ;
        return true;
    }
}

StateFunction( setDistGreen )
{
    entryState
    {
        sm.setTimeout( 30000 ) ;
    }
    onState
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();

        else
        {  
            setting.distGreen = map( adcSample, 0, 1023, 0, 180) ;
            semaphore.write( setting.distGreen ) ;
        }
    }
    exitState
    {  
        EEPROM.write( GREEN_DIST_POS, setting.distGreen ) ;
        return 1 ;
    }
}

StateFunction( setDistRed )
{
    entryState
    {
        sm.setTimeout( 30000 ) ;
    }
    onState
    {
        if( adcSample < 10 || sm.timeout() ) sm.exit();

        else
        {  
            setting.distRed = map( setting.distRed, 0, 1023, 0, 180) ;
            semaphore.write( setting.distRed ) ;
        }
    }
    exitState
    {
        EEPROM.write( RED_DIST_POS, setting.distRed ) ;
        return 1 ;
    }
}

// STATE MACHINE
uint8 teachIn(void)
{
    REPEAT_MS( 20 )
    {
        adcSample = analogRead( configPin ) ;
    } END_REPEAT

    STATE_MACHINE_BEGIN( sm )

        State( waitButtonPress ) {
            sm.nextState(adjustGreenBrightness, 1000) ; }

        State( adjustGreenBrightness ) {
            if( sm.timeoutError() )            sm.nextState( waitButtonPress,          1000 ) ;
            else if( signal.type == TWO_TONE ) sm.nextState( adjustRedBrightness,      1000 ) ;      // two tone only has green and red
            else                               sm.nextState( adjustYellowBrightness,   1000 ) ; }    // three or four tone

        State( adjustYellowBrightness ) {
            if( sm.timeoutError() )             sm.nextState( waitButtonPress,         1000 ) ;    
            else if( signal.type == FOUR_TONE ) sm.nextState( adjustYellow2Brightness, 1000 ) ;     // four tone, adjust the 2nd yellow led
            else                                sm.nextState( adjustRedBrightness,     1000 ) ; }   // three tone, adjust the red led

        State( adjustYellow2Brightness ) {
            if( sm.timeoutError() ) sm.nextState( waitButtonPress,     1000 ) ;    
            else                    sm.nextState( adjustRedBrightness, 1000 ) ; }

        State( adjustRedBrightness ) {
            if( sm.timeoutError() ) sm.nextState( waitButtonPress, 1000 ) ;
            else                    sm.nextState( setMainGreen,    1000 ) ; }

        State( setMainGreen ) {
            if( sm.timeoutError() ) sm.nextState( waitButtonPress, 1000 ) ; 
            else                    sm.nextState( setMainRed,      1000 ) ; }
        
        State( setMainRed ) {
            if( sm.timeoutError() ) sm.nextState( waitButtonPress, 1000 ) ; 
            else                    sm.nextState( setDistGreen,    1000 ) ; }

        State( setDistGreen ) {
            if( sm.timeoutError() ) sm.nextState( waitButtonPress, 1000 ) ; 
            else                    sm.nextState( setDistRed,      1000 ) ; }
        
        State( setDistRed ) {
            sm.nextState( waitButtonPress, 1000 ) ; }


    STATE_MACHINE_END( sm ) ;
}