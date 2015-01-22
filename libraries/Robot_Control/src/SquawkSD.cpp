#include <SquawkSD.h>

SquawkSynthSD SquawkSD;

class StreamFile : public SquawkStream {
  private:
    Fat16 f;
	public:
		StreamFile(Fat16 file = Fat16()) { f = file; }
    uint8_t read() { return f.read(); }
    void seek(size_t offset) { f.seekSet(offset); }
};

static StreamFile file;

extern const uint16_t period_tbl[84] PROGMEM;

void SquawkSynthSD::play(Fat16 melody) {
	SquawkSynth::pause();
	file = StreamFile(melody);
	SquawkSynth::play(&file);
}

/*
void SquawkSynthSD::convert(Fat16 in, Fat16 out) {
  unsigned int n;
  uint8_t patterns = 0, order_count;
  unsigned int ptn, row, chn;
  uint8_t temp;

  uint8_t fxc[4], fxp[4], note[4], sample[4];
  uint16_t period;

  out.write('S'); // ID
  out.write('Q');
  out.write('M');
  out.write('1');
  out.write((uint8_t)0); // No meta data
  out.write((uint8_t)0);
  
  // Write order list, count patterns
  in.seek(0x3B6);
  order_count = in.read();
  out.write(order_count);
  in.seek(0x3B8);
  for(n = 0; n < order_count; n++) {
    temp = in.read();
    if(temp >= patterns) patterns = temp + 1;
    out.write(temp);
  }
  
  // Write patterns
  in.seek(0x43C);
  for(ptn = 0; ptn < patterns; ptn++) {
    for(row = 0; row < 64; row++) {
      for(chn = 0; chn < 4; chn++) {
        
        // Basic extraction
        temp         = in.read();          // sample.msb and period.msb
        period       = (temp & 0x0F) << 8;
        sample[chn]  = temp & 0xF0;
        period      |= in.read();          // period.lsb
        temp         = in.read();          // sample.lsb and effect
        sample[chn] |= temp >> 4;
        fxc[chn]     = (temp & 0x0F) << 4;
        fxp[chn]     = in.read();          // parameters
        if(fxc[chn] == 0xE0) {
          fxc[chn]    |= fxp[chn] >> 4;    // extended parameters
          fxp[chn]    &= 0x0F;
        }
        
        #define DIF(A, B) ((A) > (B) ? ((int32_t)(A) - (int32_t)(B)) : ((int32_t)(B) - (int32_t)(A)))
        // Find closest matching period
        if(period == 0) {
          note[chn] = 0x7F;
        } else {
          int16_t best = DIF(period, pgm_read_word(&period_tbl[0]));
          note[chn] = 0;
          for(n = 0; n < sizeof(period_tbl) / sizeof(uint16_t); n++) {
            if(DIF(period, pgm_read_word(&period_tbl[n])) < best) {
              note[chn] = n;
              best = DIF(period, pgm_read_word(&period_tbl[n]));
            }
          }
        }
        
        // Crunch volume/decimal commands
        if(fxc[chn] == 0x50 || fxc[chn] == 0x60 || fxc[chn] == 0xA0) {
          fxp[chn] = (fxp[chn] >> 1) & 0x77;
        } else if(fxc[chn] == 0x70) {
          fxp[chn] = (fxp[chn] & 0xF0) | ((fxp[chn] & 0x0F) >> 1);
        } else if(fxc[chn] == 0xC0 || fxc[chn] == 0xEA || fxc[chn] == 0xEB) {
          fxp[chn] >>= 1;
        } else if(fxc[chn] == 0xD0) {
          fxp[chn] = ((fxp[chn] >> 4) * 10) | (fxp[chn] & 0x0F);
        }

        // Re-nibblify - it's a word!
        if(chn != 3) {
          if((fxc[chn] & 0xF0) == 0xE0) fxp[chn] |= fxc[chn] << 4;
          fxc[chn] >>= 4;       
        }

      }

      // Ghetto crunch the last channel to save a byte
      switch(fxc[3]) {
        case 0x50: case 0x60: case 0xA0:
          fxc[3] = 0x1;
          if((fxp[3] >> 4) >= (fxp[3] & 0x0F)) {
            fxp[3] = 0x80 + ((fxp[3] >> 4) - (fxp[3] & 0x0F));
          } else {
            fxp[3] = ((fxp[3] & 0x0F) - (fxp[3] >> 4));
          }
          break;
        case 0x70:
          fxc[3] = (fxp[3] & 0x4) ? 0x3 : 0x2;
          fxp[3] = (fxp[3] >> 4) | ((fxp[3] & 0x03) << 4);
          break;
        case 0xC0:
          fxc[3] = 0x4;
          fxp[3] &= 0x1F;
          break;
        case 0xB0:
          fxc[3] = 0x5;
          fxp[3] &= 0x1F;
          break;
        case 0xD0:
          fxc[3] = 0x6;
          if(fxp[3] > 63) fxp[3] = 0;
          break;
        case 0xF0:
          if(fxp[3] > 0x20) {
            fxc[3] = 0x0;
            fxp[3] = 0x00;
          } else {
            fxc[3] = 0x7;             
          }
          break;
        case 0xE7:
          fxc[3] = 0x8;
          break;
        case 0xE9:
          fxc[3] = 0x9;
          break;
        case 0xEA:
          fxc[3] = 0xA;
          fxp[3] |= 0x08;
          break;
        case 0xEB:
          fxc[3] = 0xA;
          break;
        case 0xEC:
          fxc[3] = 0xB;
          break;
        case 0xED:
          fxc[3] = 0xB;
          fxp[3] |= 0x10;
          break;
        case 0xEE:
          fxc[3] = 0xC;
          break;
        default:
          fxc[3] = 0;
          fxp[3] = 0;
      }
      if(note[3] != 0x7F) fxp[3] |= 0x80;
      if(sample[3]) fxp[3] |= 0x40;

      // Write out
      out.write((fxc[0]) | fxc[1] << 4);
      out.write(fxp[0]);
      out.write(fxp[1]);
      out.write((fxc[2]) | fxc[3] << 4);
      out.write(fxp[2]);
      out.write(fxp[3]);
      out.write(note[0] | (sample[0] == 0 ? 0x00 : 0x80));
      out.write(note[1] | (sample[1] == 0 ? 0x00 : 0x80));
      out.write(note[2] | (sample[2] == 0 ? 0x00 : 0x80));
    }
  }
}*/
