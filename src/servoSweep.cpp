#include "ServoSweep.h"

#define movingLeft 0
#define movingRight 1

ServoSweep::ServoSweep( uint8 _servoPin, uint8 _min, uint8 _max, uint8 _turnOff,  uint8 _speed ) {                   // constructor 1
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;
   
    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;               // start with middle position
    pos = middlePosition ;

    if( _turnOff ) turnOff = 1 ;
    else           turnOff = 0 ;
}

ServoSweep::ServoSweep( uint8 _servoPin, uint8 _min, uint8 _max, uint8 _speed, uint8 _turnOff, uint8 _relayPin ) {      // constructor 2
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

    pos = middlePosition ;

    relayPresent = 1;
    relayPin = _relayPin ;

    if( _turnOff ) turnOff = 1 ;
    else           turnOff = 0 ;
}

void ServoSweep::begin() {
    if( servoPin != 255) {
        servo.write( pos ) ;
        servo.attach( servoPin ) ;
    }

    if( relayPresent ) pinMode( relayPin, OUTPUT ) ;
}

void ServoSweep::setState( uint8 _state ) {
    state = _state ;
    if( turnOff ) servo.attach( servoPin ) ;
}

void ServoSweep::setMin( uint8 _min ) {
    servoMin = _min ;
    if( turnOff ) servo.attach( servoPin ) ;
}
void ServoSweep::setMax( uint8 _max ) {
    servoMax = _max ;
    if( turnOff ) servo.attach( servoPin ) ;
}

// void ServoSweep::turnOn() {
//     servo.attach( servoPin ) ;
// }

// void ServoSweep::turnOff() {
//     servo.detach(  ) ;
// }

uint8 ServoSweep::sweep ( ) {
    if( millis() > timeToRun ) {
        timeToRun = millis() + servoSpeed ;

        if( state ) {
            if( pos < servoMax ) pos ++ ;
        }
        else {
            if( pos > servoMin ) pos -- ;
        }


        if( prevPos != pos ) {
            prevPos  = pos ;

            if( ( pos == servoMax || pos == servoMin ) && turnOff == 1 ) servo.detach( ) ;

            if( relayPresent ) {
                if( pos < middlePosition ) digitalWrite( relayPin,  LOW ) ;
                else                       digitalWrite( relayPin, HIGH ) ;
            }
            if( servoPin != 255 ) servo.write( pos ) ;
            return pos ;
        }
        else {
            return 0 ;
        }
    }
}