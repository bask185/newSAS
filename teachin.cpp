// HEADER FILES
#include "teachIn.h"
#include <EEPROM.h>
#include "src/io.h"
#include "output.h"

#include "src/stateMachineClass.h"

#define entryState  if( sm.entryState() )
#define onState     if( sm.onState() )
#define exitState   if( sm.exitState() == 0 ) return 0 ; \
                    else

static StateMachine sm ;
uint8 pwm ;


enum EEaddresses {
    PWM_GREEN_ADDR = 0x00,
    PWM_YELLOW_ADDR,
    PWM_RED_ADDR,
    GREEN_SERVO_POS,
    RED_SERVO_POS,
    INIT_ADDR,
}; 

uint16 adcSample ;

// VARIABLES

// FUNCTIONS
extern void teachInInit(void)
{ 
    sm.setState( waitButtonPress, 0 ) ;

    uint8_t firstEntry = EEPROM.read( INIT_ADDR ) ;

    if( firstEntry != 0xCC )  // is signal is already started once, retreive values
    {
        EEPROM.write( GREEN_SERVO_POS,   90 ) ;
        EEPROM.write( RED_SERVO_POS,    135 ) ;
        EEPROM.write( PWM_GREEN_ADDR,   255 ) ;
        EEPROM.write( PWM_YELLOW_ADDR,  255 ) ;
        EEPROM.write( PWM_RED_ADDR,     255 ) ;

        EEPROM.write( INIT_ADDR, 0xCC) ;
        Serial.println("FIRST TIME BOOTING SAS SOFTWARE V1.O, DEFAULT SETTINGS ARE LOADED") ;
    }

    servoPosMin   =    EEPROM.read( GREEN_SERVO_POS ) ; Serial.println(servoPosMin);
    servoPosMax   =    EEPROM.read( RED_SERVO_POS )   ; Serial.println(servoPosMax);
    // pwm  =    EEPROM.read( PWM_GREEN_ADDR )  ; Serial.println(pwm );
    // pwm =    EEPROM.read( PWM_YELLOW_ADDR ) ; Serial.println(pwm);
    // pwm    =    EEPROM.read( PWM_RED_ADDR )    ; Serial.println(pwm);
}

// STATE FUNCTIONS

StateFunction( waitButtonPress )// just wait on the first button press
{ 
    entryState 
    {
        
    }
    onState 
    {
        if( adcSample <= 3 ) sm.exit() ;        
    }
    exitState 
    {

        return true;
    }
}

// StateFunction( adjustGreenBrightness )
// {
//     entryState
//     {
//         digitalWrite( greenLedPin,   HIGH ) ;
//         digitalWrite( yellowLedPin,  LOW ) ;
//         digitalWrite( yellowLedPin2, LOW ) ;
//         digitalWrite( redLedPin,     LOW ) ;
//         sm.setTimeout( 30000 ) ;
//     }
//     onState
//     {
//         if( val < 10 || sm.timeout() ) sm.exit(); // if timeout or button press occurs, exit
//         else                           pwm = map( adcSample, 0, 1023, 0, 255 ) ;
        
//         analogWrite( greenLedPin, pwm ) ;
//     }
//     exitState
//     {
//         EEPROM.write( PWM_GREEN_ADDR, pwm ) ;
//         digitalWrite( greenLedPin, LOW ) ;
//         return true;
//     }
// }

// StateFunction( adjustYellowBrightness )
// {
//     entryState 
//     {
//         digitalWrite( yellowLedPin, HIGH ) ;
//         sm.setTimeout( 30000 ) ;
//     }
//     onState 
//     {
//             if( val < 10 || sm.timeout() ) sm.exit();
//             else                           pwm = map ( adcSample, 0, 1023, 0, 255 ) ;

//             analogWrite(yellowLedPin, pwm);
//         }
//     }
//     exitState 
//     {
//         digitalWrite(yellowLedPin, 0);
//         EEPROM.write( PWM_YELLOW_ADDR, pwm  ) ;
//         return true;
//     }
// }

// StateFunction( adjustYellowBrightness2 )
// {
//     entryState 
//     {
//         digitalWrite( yellowLedPin2, HIGH ) ;
//         sm.setTimeout( 30000 ) ;
//     }
//     onState 
//     {
//             if( val < 10 || sm.timeout() ) sm.exit();
//             else                           pwm = map ( adcSample, 0, 1023, 0, 255 ) ;

//             analogWrite( yellowLedPin2, pwm);
//         }
//     }
//     exitState 
//     {
//         digitalWrite( yellowLedPin2, LOW );
//         EEPROM.write( PWM_YELLOW_ADDR, pwm  ) ;
//         return true;
//     }
// }


// StateFunction( adjustRedBrightness )
// {
//     entryState 
//     {
//         digitalWrite( redLedPin, HIGH ) ;
//         sm.setTimeout( 30000 ) ;
//     }
//     onState
//     {
//         if( adcSample < 10 || sm.timeout() ) sm.exit();
//         else                                 pwm = map ( adcSample, 0, 1023, 0, 255 ) ;

//         analogWrite( redLedPin, pwm ) ;
//     }
//     exitState
//     {
//         digitalWrite( redLedPin, LOW ) ;
//         EEPROM.write( PWM_RED_ADDR, pwm ) ;
//         return true;
//     }
// }



StateFunction( setServoGreen )
{
    entryState
    {
        sm.setTimeout( 30000 ) ;
        semaphore.attach( servoPin_ );
        digitalWrite( greenLedPin, HIGH ) ;
    }
    onState
    {
        if( val < 10 || sm.timeout() ) sm.exit();

        else
        {  
            servoPosMax = map( adcSample, 0, 1023, 0, 180) ;
            semaphore.write( servoPosMax ) ;
        }
    }
    exitState
    {
        EEPROM.write( GREEN_SERVO_POS, servoPosMin ) ;
        semaphore.detach();
        digitalWrite( greenLedPin, LOW ) ;
        return true;
    }
}



StateFunction( setServoRed )
{
    uint16_t val;

    entryState
    { 
        sm.setTimeout( 30000 ) ;
        semaphore.attach( servoPin_ );
        digitalWrite( redLedPin, HIGH ) ;
    }

    onState
    {
        if( val < 10 || sm.timeout() ) sm.exit();

        else
        {   
            servoPosMin = map( adcSample, 0, 1023, 0, 180) ;
            semaphore.write( servoPosMin ) ;
        }
    }

    exitState
    {
        EEPROM.write( RED_SERVO_POS, servoPosMin ) ;
        semaphore.detach();
        digitalWrite( redLedPin, LOW ) ;

        return true;
    }
}


// STATE MACHINE
extern bool teachIn(void)
{
    REPEAT_MS( 20 )
    {
        adcSample = analogRead( configPin ) ;
    } END_REPEAT

    STATE_MACHINE_BEGIN( sm )

        State( waitButtonPress ) {
            //nextState(adjustGreenBrightness, 100) ; }
            nextState(setServoGreen, 1000) ; }

        // State( adjustGreenBrightness ) {
        //     if( sm.timeoutError() )            sm.nextState( waitButtonPress,           0 ) ;
        //     else if( signal.type == TWO_TONE ) sm.nextState( adjustRedBrightness,    1000 ) ;      // two tone only has green and red
        //     else                               sm.nextState( adjustYellowBrightness, 1000 ) ; }    // three or four tone

        // State( adjustYellowBrightness ) {
        //     if( sm.timeoutError() )             sm.nextState( waitButtonPress,            0 ) ;    
        //     else if( signal.type == FOUR_TONE ) sm.nextState( adjustYellowBrightness2, 1000 ) ;     // four tone, adjust the 2nd yellow led
        //     else                                sm.nextState( adjustRedBrightness,     1000 ) ; }   // three tone, adjust the red led

        // State( adjustYellowBrightness2 ) {
        //     if( sm.timeoutError() ) sm.nextState( waitButtonPress,            0 ) ;    
        //     else                    sm.nextState( adjustRedBrightness,     1000 ) ; }

        // State( adjustRedBrightness ) {
        //     if( sm.timeoutError() ) sm.nextState( waitButtonPress, 100 ) ;
        //     else                    sm.nextState( setServoGreen,   100 ) ; }

        State( setServoGreen ) {
            if( sm.timeoutError() ) sm.nextState( waitButtonPress, 0 ) ; 
            else                    sm.nextState( setServoRed,  1000 ) ; }
        
        State( setServoRed ) {
            sm.nextState( waitButtonPress, 1000 ) ;

    STATE_MACHINE_END(sm)
}