// Squawk Soft-Synthesizer Library for Arduino
//
// Davey Taylor 2013
// d.taylor@arduino.cc

#include "Squawk.h"

// Period range, used for clamping
#define PERIOD_MIN 28
#define PERIOD_MAX 3424

// Convenience macros
#define LO4(V)    ((V) & 0x0F)
#define HI4(V)    (((V) & 0xF0) >> 4)
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define FREQ(PERIOD) (tuning_long / (PERIOD))

// SquawkStream class for PROGMEM data
class StreamROM : public SquawkStream {
  private:
    uint8_t *p_start;
    uint8_t *p_cursor;
  public:
    StreamROM(const uint8_t *p_rom = NULL) { p_start = p_cursor = (uint8_t*)p_rom; }
    uint8_t read() { return pgm_read_byte(p_cursor++); }
    void seek(size_t offset) { p_cursor = p_start + offset; }
};

// Oscillator memory
typedef struct {
  uint8_t fxp;
  uint8_t offset;
  uint8_t mode;
} pto_t;

// Deconstructed cell
typedef struct {
  uint8_t fxc, fxp, ixp;
} cel_t;

// Effect memory
typedef struct {
  int8_t    volume;
  uint8_t   port_speed;
  uint16_t  port_target;
  bool      glissando;
  pto_t     vibr;
  pto_t     trem;
  uint16_t  period;
  uint8_t   param;
} fxm_t;

// Locals
static uint8_t  order_count;
static uint8_t  order[64];
static uint8_t  speed;
static uint8_t  tick;
static uint8_t  ix_row;
static uint8_t  ix_order;
static uint8_t  ix_nextrow;
static uint8_t  ix_nextorder;
static uint8_t  row_delay;
static fxm_t    fxm[4];
static cel_t    cel[4];
static uint32_t tuning_long;
static uint16_t sample_rate;
static float    tuning = 1.0;
static uint16_t tick_rate = 50;

static SquawkStream *stream;
static uint16_t stream_base;
static StreamROM rom;

// Imports
extern intptr_t squawk_register;
extern uint16_t cia;

// Exports
osc_t osc[4];
uint8_t pcm = 128;

// ProTracker period tables
const uint16_t period_tbl[84] PROGMEM = {
  3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032, 1920, 1814,
  1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016,  960,  907,
   856,  808,  762,  720,  678,  640,  604,  570,  538,  508,  480,  453,
   428,  404,  381,  360,  339,  320,  302,  285,  269,  254,  240,  226,
   214,  202,  190,  180,  170,  160,  151,  143,  135,  127,  120,  113,
   107,  101,   95,   90,   85,   80,   75,   71,   67,   63,   60,   56,
    53,   50,   47,   45,   42,   40,   37,   35,   33,   31,   30,   28,
};

// ProTracker sine table
const int8_t sine_tbl[32] PROGMEM = {
  0x00, 0x0C, 0x18, 0x25, 0x30, 0x3C, 0x47, 0x51, 0x5A, 0x62, 0x6A, 0x70, 0x76, 0x7A, 0x7D, 0x7F,
  0x7F, 0x7F, 0x7D, 0x7A, 0x76, 0x70, 0x6A, 0x62, 0x5A, 0x51, 0x47, 0x3C, 0x30, 0x25, 0x18, 0x0C,
};

// Squawk object
SquawkSynth Squawk;

// Look up or generate waveform for ProTracker vibrato/tremolo oscillator
static int8_t do_osc(pto_t *p_osc) {
  int8_t sample = 0;
  int16_t mul;
  switch(p_osc->mode & 0x03) {
    case 0: // Sine
      sample = pgm_read_byte(&sine_tbl[(p_osc->offset) & 0x1F]);
      if(p_osc->offset & 0x20) sample = -sample;
      break;
    case 1: // Square
      sample = (p_osc->offset & 0x20) ? 127 : -128;
      break;
    case 2: // Saw
      sample = -(p_osc->offset << 2);
      break;
    case 3: // Noise (random)
      sample = rand();
      break;
  }
  mul = sample * LO4(p_osc->fxp);
  p_osc->offset = (p_osc->offset + HI4(p_osc->fxp));
  return mul >> 6;
}

// Calculates and returns arpeggio period
// Essentially finds period of current note + halftones
static inline uint16_t arpeggio(uint8_t ch, uint8_t halftones) {
  uint8_t n;
  for(n = 0; n != 47; n++) {
    if(fxm[ch].period >= pgm_read_word(&period_tbl[n])) break;
  }
  return pgm_read_word(&period_tbl[MIN(n + halftones, 47)]);
}

// Calculates and returns glissando period
// Essentially snaps a sliding frequency to the closest note
static inline uint16_t glissando(uint8_t ch) {
  uint8_t n;
  uint16_t period_h, period_l;
  for(n = 0; n != 47; n++) {
    period_l = pgm_read_word(&period_tbl[n]);
    period_h = pgm_read_word(&period_tbl[n + 1]);
    if(fxm[ch].period < period_l && fxm[ch].period >= period_h) {
      if(period_l - fxm[ch].period <= fxm[ch].period - period_h) {
        period_h = period_l;
      }
      break;
    }
  }
  return period_h;
}

// Tunes Squawk to a different frequency
void SquawkSynth::tune(float new_tuning) {
  tuning = new_tuning;
  tuning_long = (long)(((double)3669213184.0 / (double)sample_rate) * (double)tuning);

}

// Sets tempo
void SquawkSynth::tempo(uint16_t new_tempo) {
  tick_rate = new_tempo;
  cia = sample_rate / tick_rate; // not atomic?
}

// Initializes Squawk
// Sets up the selected port, and the sample grinding ISR
void SquawkSynth::begin(uint16_t hz) {
  word isr_rr;

  sample_rate = hz;
  tuning_long = (long)(((double)3669213184.0 / (double)sample_rate) * (double)tuning);
  cia = sample_rate / tick_rate;

  if(squawk_register == (intptr_t)&OCR0A) {
    // Squawk uses PWM on OCR0A/PD5(ATMega328/168)/PB7(ATMega32U4)
#ifdef  __AVR_ATmega32U4__
    DDRB  |= 0b10000000; // TODO: FAIL on 32U4
#else
    DDRD  |= 0b01000000;
#endif
    TCCR0A = 0b10000011; // Fast-PWM 8-bit
    TCCR0B = 0b00000001; // 62500Hz
    OCR0A  = 0x7F;
  } else if(squawk_register == (intptr_t)&OCR0B) {
    // Squawk uses PWM on OCR0B/PC5(ATMega328/168)/PD0(ATMega32U4)
#ifdef  __AVR_ATmega32U4__
    DDRD  |= 0b00000001;
#else
    DDRD  |= 0b00100000;
#endif                   // Set timer mode to
    TCCR0A = 0b00100011; // Fast-PWM 8-bit
    TCCR0B = 0b00000001; // 62500Hz
    OCR0B  = 0x7F;
#ifdef OCR2A
  } else if(squawk_register == (intptr_t)&OCR2A) {
    // Squawk uses PWM on OCR2A/PB3
    DDRB  |= 0b00001000; // Set timer mode to
    TCCR2A = 0b10000011; // Fast-PWM 8-bit
    TCCR2B = 0b00000001; // 62500Hz
    OCR2A  = 0x7F;
#endif
#ifdef OCR2B
  } else if(squawk_register == (intptr_t)&OCR2B) {
    // Squawk uses PWM on OCR2B/PD3
    DDRD  |= 0b00001000; // Set timer mode to
    TCCR2A = 0b00100011; // Fast-PWM 8-bit
    TCCR2B = 0b00000001; // 62500Hz
    OCR2B  = 0x7F;
#endif
#ifdef OCR3AL
  } else if(squawk_register == (intptr_t)&OCR3AL) {
    // Squawk uses PWM on OCR3AL/PC6
    DDRC  |= 0b01000000; // Set timer mode to
    TCCR3A = 0b10000001; // Fast-PWM 8-bit
    TCCR3B = 0b00001001; // 62500Hz
    OCR3AH = 0x00;
    OCR3AL = 0x7F;
#endif
  } else if(squawk_register == (intptr_t)&SPDR) {
    // NOT YET SUPPORTED
    // Squawk uses external DAC via SPI
    // TODO: Configure SPI
    // TODO: Needs SS toggle in sample grinder
  } else if(squawk_register == (intptr_t)&PORTB) {
    // NOT YET SUPPORTED
    // Squawk uses resistor ladder on PORTB
    // TODO: Needs shift right in sample grinder
    DDRB   = 0b11111111;
  } else if(squawk_register == (intptr_t)&PORTC) {
    // NOT YET SUPPORTED
    // Squawk uses resistor ladder on PORTC
    // TODO: Needs shift right in sample grinder
    DDRC   = 0b11111111;
  }

  // Seed LFSR (needed for noise)
  osc[3].freq = 0x2000;

  // Set up ISR to run at sample_rate (may not be exact)
  isr_rr = F_CPU / sample_rate;
  TCCR1A = 0b00000000;     // Set timer mode
  TCCR1B = 0b00001001;
  OCR1AH = isr_rr >> 8;    // Set freq
  OCR1AL = isr_rr & 0xFF;
}

// Decrunches a 9 byte row into a useful data
static void decrunch_row() {
  uint8_t data;

  // Initial decrunch
  stream->seek(stream_base + ((order[ix_order] << 6) + ix_row) * 9);
  data = stream->read(); cel[0].fxc  =  data << 0x04;
                         cel[1].fxc  =  data &  0xF0;
  data = stream->read(); cel[0].fxp  =  data;
  data = stream->read(); cel[1].fxp  =  data;
  data = stream->read(); cel[2].fxc  =  data << 0x04;
                         cel[3].fxc  =  data >> 0x04;
  data = stream->read(); cel[2].fxp  =  data;
  data = stream->read(); cel[3].fxp  =  data;
  data = stream->read(); cel[0].ixp  =  data;
  data = stream->read(); cel[1].ixp  =  data;
  data = stream->read(); cel[2].ixp  =  data;

  // Decrunch extended effects
  if(cel[0].fxc == 0xE0) { cel[0].fxc |= cel[0].fxp >> 4; cel[0].fxp &= 0x0F; }
  if(cel[1].fxc == 0xE0) { cel[1].fxc |= cel[1].fxp >> 4; cel[1].fxp &= 0x0F; }
  if(cel[2].fxc == 0xE0) { cel[2].fxc |= cel[2].fxp >> 4; cel[2].fxp &= 0x0F; }

  // Decrunch cell 3 ghetto-style
  cel[3].ixp = ((cel[3].fxp & 0x80) ? 0x00 : 0x7F) | ((cel[3].fxp & 0x40) ? 0x80 : 0x00);
  cel[3].fxp &= 0x3F;
  switch(cel[3].fxc) {
    case 0x02:
    case 0x03: if(cel[3].fxc & 0x01) cel[3].fxp |= 0x40; cel[3].fxp = (cel[3].fxp >> 4) | (cel[3].fxp << 4); cel[3].fxc = 0x70; break;
    case 0x01: if(cel[3].fxp & 0x08) cel[3].fxp = (cel[3].fxp & 0x07) << 4; cel[3].fxc = 0xA0; break;
    case 0x04: cel[3].fxc = 0xC0; break;
    case 0x05: cel[3].fxc = 0xB0; break;
    case 0x06: cel[3].fxc = 0xD0; break;
    case 0x07: cel[3].fxc = 0xF0; break;
    case 0x08: cel[3].fxc = 0xE7; break;
    case 0x09: cel[3].fxc = 0xE9; break;
    case 0x0A: cel[3].fxc = (cel[3].fxp & 0x08) ? 0xEA : 0xEB; cel[3].fxp &= 0x07; break;
    case 0x0B: cel[3].fxc = (cel[3].fxp & 0x10) ? 0xED : 0xEC; cel[3].fxp &= 0x0F; break;
    case 0x0C: cel[3].fxc = 0xEE; break;
  }

  // Apply generic effect parameter memory
  uint8_t ch;
  cel_t *p_cel = cel;
  fxm_t *p_fxm = fxm;
  for(ch = 0; ch != 4; ch++) {
    uint8_t fx = p_cel->fxc;
    if(fx == 0x10 || fx == 0x20 || fx == 0xE1 || fx == 0xE2 || fx == 0x50 || fx == 0x60 || fx == 0xA0) {
      if(p_cel->fxp) {
        p_fxm->param = p_cel->fxp;
      } else {
        p_cel->fxp = p_fxm->param;
      }
    }
    p_cel++; p_fxm++;
  }
}

// Resets playback
static void playroutine_reset() {
  memset(fxm, 0, sizeof(fxm));
  tick         = 0;
  ix_row       = 0;
  ix_order     = 0;
  ix_nextrow   = 0xFF;
  ix_nextorder = 0xFF;
  row_delay    = 0;
  speed        = 6;
  decrunch_row();
}

// Start grinding samples
void SquawkSynth::play() {
  TIMSK1 = 1 << OCIE1A; // Enable interrupt
}

// Load a melody stream and start grinding samples
void SquawkSynth::play(SquawkStream *melody) {
  uint8_t n;
  pause();
  stream = melody;
  stream->seek(0);
  n = stream->read();
  if(n == 'S') {
    // Squawk SD file
    stream->seek(4);
    stream_base = stream->read() << 8;
    stream_base |= stream->read();
    stream_base += 6;
  } else {
    // Squawk ROM array
    stream_base = 1;
  }
  stream->seek(stream_base);
  order_count = stream->read();
  if(order_count <= 64) {
    stream_base += order_count + 1;
    for(n = 0; n < order_count; n++) order[n] = stream->read();
    playroutine_reset();
    play();
  } else {
    order_count = 0;
  }
}

// Load a melody in PROGMEM and start grinding samples
void SquawkSynth::play(const uint8_t *melody) {
  pause();
  rom = StreamROM(melody);
  play(&rom);
}

// Pause playback
void SquawkSynth::pause() {
  TIMSK1 = 0; // Disable interrupt
}

// Stop playing, unload melody
void SquawkSynth::stop() {
  pause();
  order_count = 0; // Unload melody
}

// Progress module by one tick
void squawk_playroutine() {
  static bool lockout = false;

  if(!order_count) return;

  // Protect from re-entry via ISR
  cli();
  if(lockout) {
    sei();
    return;
  }
  lockout = true;
  sei();

  // Handle row delay
  if(row_delay) {
    if(tick == 0) row_delay--;
    // Advance tick
    if(++tick == speed) tick = 0;
  } else {

    // Quick pointer access
    fxm_t *p_fxm = fxm;
    osc_t *p_osc = osc;
    cel_t *p_cel = cel;

    // Temps
    uint8_t ch, fx, fxp;
    bool    pattern_jump = false;
    uint8_t ix_period;

    for(ch = 0; ch != 4; ch++) {
      uint8_t       temp;

      // Local register copy
      fx        = p_cel->fxc;
      fxp       = p_cel->fxp;
      ix_period = p_cel->ixp;

      // If first tick
      if(tick == (fx == 0xED ? fxp : 0)) {

        // Reset volume
        if(ix_period & 0x80) p_osc->vol = p_fxm->volume = 0x20;

        if((ix_period & 0x7F) != 0x7F) {

          // Reset oscillators (unless continous flag set)
          if((p_fxm->vibr.mode & 0x4) == 0x0) p_fxm->vibr.offset = 0;
          if((p_fxm->trem.mode & 0x4) == 0x0) p_fxm->trem.offset = 0;

          // Cell has note
          if(fx == 0x30 || fx == 0x50) {

            // Tone-portamento effect setup
            p_fxm->port_target = pgm_read_word(&period_tbl[ix_period & 0x7F]);
          } else {

            // Set required effect memory parameters
            p_fxm->period = pgm_read_word(&period_tbl[ix_period & 0x7F]);

            // Start note
            if(ch != 3) p_osc->freq = FREQ(p_fxm->period);

          }
        }

        // Effects processed when tick = 0
        switch(fx) {
          case 0x30: // Portamento
            if(fxp) p_fxm->port_speed = fxp;
            break;
          case 0xB0: // Jump to pattern
            ix_nextorder = (fxp >= order_count ? 0x00 : fxp);
            ix_nextrow = 0;
            pattern_jump = true;
            break;
          case 0xC0: // Set volume
            p_osc->vol = p_fxm->volume = MIN(fxp, 0x20);
            break;
          case 0xD0: // Jump to row
            if(!pattern_jump) ix_nextorder = ((ix_order + 1) >= order_count ? 0x00 : ix_order + 1);
            pattern_jump = true;
            ix_nextrow = (fxp > 63 ? 0 : fxp);
            break;
          case 0xF0: // Set speed, BPM(CIA) not supported
            if(fxp <= 0x20) speed = fxp;
            break;
          case 0x40: // Vibrato
            if(fxp) p_fxm->vibr.fxp = fxp;
            break;
          case 0x70: // Tremolo
            if(fxp) p_fxm->trem.fxp = fxp;
            break;
          case 0xE1: // Fine slide up
            if(ch != 3) {
              p_fxm->period = MAX(p_fxm->period - fxp, PERIOD_MIN);
              p_osc->freq = FREQ(p_fxm->period);
            }
            break;
          case 0xE2: // Fine slide down
            if(ch != 3) {
              p_fxm->period = MIN(p_fxm->period + fxp, PERIOD_MAX);
              p_osc->freq = FREQ(p_fxm->period);
            }
            break;
          case 0xE3: // Glissando control
            p_fxm->glissando = (fxp != 0);
            break;
          case 0xE4: // Set vibrato waveform
            p_fxm->vibr.mode = fxp;
            break;
          case 0xE7: // Set tremolo waveform
            p_fxm->trem.mode = fxp;
            break;
          case 0xEA: // Fine volume slide up
            p_osc->vol = p_fxm->volume = MIN(p_fxm->volume + fxp, 0x20);
            break;
          case 0xEB: // Fine volume slide down
            p_osc->vol = p_fxm->volume = MAX(p_fxm->volume - fxp, 0);
            break;
          case 0xEE: // Delay
            row_delay = fxp;
            break;
        }
      } else {

        // Effects processed when tick > 0
        switch(fx) {
          case 0x10: // Slide up
            if(ch != 3) {
              p_fxm->period = MAX(p_fxm->period - fxp, PERIOD_MIN);
              p_osc->freq = FREQ(p_fxm->period);
            }
            break;
          case 0x20: // Slide down
            if(ch != 3) {
              p_fxm->period = MIN(p_fxm->period + fxp, PERIOD_MAX);
              p_osc->freq = FREQ(p_fxm->period);
            }
            break;
/*
          // Just feels... ugly
          case 0xE9: // Retrigger note
            temp = tick; while(temp >= fxp) temp -= fxp;
            if(!temp) {
              if(ch == 3) {
                p_osc->freq = p_osc->phase = 0x2000;
              } else {
                p_osc->phase = 0;
              }
            }
            break;
*/            
          case 0xEC: // Note cut
            if(fxp == tick) p_osc->vol = 0x00;
            break;
          default:   // Multi-effect processing

            // Portamento
            if(ch != 3 && (fx == 0x30 || fx == 0x50)) {
              if(p_fxm->period < p_fxm->port_target) p_fxm->period = MIN(p_fxm->period + p_fxm->port_speed,  p_fxm->port_target);
              else                                   p_fxm->period = MAX(p_fxm->period - p_fxm->port_speed,  p_fxm->port_target);
              if(p_fxm->glissando) p_osc->freq = FREQ(glissando(ch));
              else                 p_osc->freq = FREQ(p_fxm->period);
            }

            // Volume slide
            if(fx == 0x50 || fx == 0x60 || fx == 0xA0) {
              if((fxp & 0xF0) == 0) p_fxm->volume -= (LO4(fxp));
              if((fxp & 0x0F) == 0) p_fxm->volume += (HI4(fxp));
              p_osc->vol = p_fxm->volume = MAX(MIN(p_fxm->volume, 0x20), 0);
            }
        }
      }

      // Normal play and arpeggio
      if(fx == 0x00) {
        if(ch != 3) {
          temp = tick; while(temp > 2) temp -= 2;
          if(temp == 0) {

            // Reset
            p_osc->freq = FREQ(p_fxm->period);
          } else if(fxp) {

            // Arpeggio
            p_osc->freq = FREQ(arpeggio(ch, (temp == 1 ? HI4(fxp) : LO4(fxp))));
          }
        }
      } else if(fx == 0x40 || fx == 0x60) {

        // Vibrato
        if(ch != 3) p_osc->freq = FREQ((p_fxm->period + do_osc(&p_fxm->vibr)));
      } else if(fx == 0x70) {
        int8_t trem = p_fxm->volume + do_osc(&p_fxm->trem);
        p_osc->vol = MAX(MIN(trem, 0x20), 0);
      }

      // Next channel
      p_fxm++; p_cel++; p_osc++;
    }

    // Advance tick
    if(++tick == speed) tick = 0;

    // Advance playback
    if(tick == 0) {
      if(++ix_row == 64) {
        ix_row = 0;
        if(++ix_order >= order_count) ix_order = 0;
      }
	    // Forced order/row
	    if( ix_nextorder != 0xFF ) {
	      ix_order = ix_nextorder;
	      ix_nextorder = 0xFF;
	    }
	    if( ix_nextrow != 0xFF ) {
	      ix_row = ix_nextrow;
	      ix_nextrow = 0xFF;
	    }
			decrunch_row();
    }

  }

  lockout = false;
}
