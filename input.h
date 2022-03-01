#include <Arduino.h>
#include "src/macros.h"

extern uint8 detectorState ;
extern uint8 greenButtonState ;
extern uint8 yellowButtonState ;
extern uint8 redButtonState ;
extern uint8 receiveState ;
extern uint8 directionState ;

const int   greenFreq =  50 ;
const int  yellowFreq = 100 ; 
const int yellowFreq1 = 150 ; 
const int     redFreq = 200 ;

extern uint8 rxFreq ;

extern  void debounceInputs() ;
extern uint8 fallTimeControl() ;

enum constants
{
    green,
    yellow, // yellow1 needed
    red,
    off,
    TWO_TONE,
    THREE_TONE,
	OCCUPIED,
	FREE,
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
