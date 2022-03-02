#include <Arduino.h>
#include <Servo.h>
#include "macros.h"

class ServoSweep {

public:
    ServoSweep( uint8 _servoPin, uint8 _min, uint8 _max, uint8 _speed, uint8 _turnOff  ) ;        // constructor 1
    ServoSweep( uint8 _servoPin, uint8 _min, uint8 _max, uint8 _speed, uint8 _turnOff, uint8 _relayPin ) ;  // constructor 2
    uint8 sweep( );
    void setState( uint8 _state );
    void setMin( uint8 _min ) ;
    void setMax( uint8 _max ) ;
    // void turnOn() ;
    void write( uint8 ) ;
    void begin( );

private:
    Servo servo ;
    unsigned long timeToRun ;
    uint8 pos ;
    uint8 state ;
    uint8 prevPos;
    uint8 servoPin ;
    uint8 servoSpeed ;
    uint8 servoMin ;
    uint8 servoMax  ;
    uint8 middlePosition ;
    uint8 relayPresent ;
    uint8 relayPin ;
    uint8 turnOff ;
    unsigned long currentTime = millis() ;
} ;
