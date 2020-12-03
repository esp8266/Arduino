// Wrapper to include both versions of the waveform generator

#ifdef WAVEFORM_LOCKED_PHASE
    #include "core_esp8266_waveform_phase.h"
#else
    #include "core_esp8266_waveform_pwm.h"
#endif
