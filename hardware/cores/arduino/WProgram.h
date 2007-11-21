#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/interrupt.h>
#include <avr/signal.h>

#include "wiring.h"

#ifdef __cplusplus
#include "HardwareSerial.h"

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned int);
long map(long, long, long, long, long);
#endif
