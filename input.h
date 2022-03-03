#include <Arduino.h>
#include "src/macros.h"

// #define NEW_SIGNAL

extern uint8 detectorState ;
extern uint8 greenButtonState ;
extern uint8 yellowButtonState ;
extern uint8 redButtonState ;
extern uint8 receiveState ;
extern uint8 directionState ;
extern uint8 buttonState ;
extern uint8 override ;
extern  uint8 fallTimeOn ;

const int   greenFreq =  50 ;
const int  yellowFreq = 100 ; 
const int yellow2Freq = 150 ; 
const int     redFreq = 200 ;

extern uint8 rxFreq ;

extern  void debounceInputs() ;
extern uint8 fallTimeControl() ;
extern  void readIncFreq() ;
extern  void readDipSwitches() ;

enum
{
    green,
    yellow,
    yellow2,
    red,
    off,
} ;

enum
{
    TWO_TONE,
    THREE_TONE,
    FOUR_TONE,
    // room for one more
} ;

enum
{
	FREE,
	OCCUPIED,
} ;    

typedef struct
{
    uint8 aspect ;
    uint8 brakeModule ;
    uint8 type ;
    uint8 track ;
    uint8 passBehind ;
} Signals ;

extern Signals signal ;
