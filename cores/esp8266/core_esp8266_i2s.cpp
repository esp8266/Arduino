/* 
  i2s.c - Software I2S library for esp8266
  
  Code taken and reworked from espessif's I2S example
  
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "Arduino.h"
#include "osapi.h"
#include "ets_sys.h"
#include "i2s_reg.h"
#include "i2s.h"

extern "C" {

#define SLC_BUF_CNT (8)  // Number of buffers in the I2S circular buffer
#define SLC_BUF_LEN (64) // Length of one buffer, in 32-bit words.

// We use a queue to keep track of the DMA buffers that are empty. The ISR
// will push buffers to the back of the queue, the I2S transmitter will pull
// them from the front and fill them. For ease, the queue will contain
// *pointers* to the DMA buffers, not the data itself. The queue depth is
// one smaller than the amount of buffers we have, because there's always a
// buffer that is being used by the DMA subsystem *right now* and we don't
// want to be able to write to that simultaneously.

// For RX, it's a little different.  The buffers in i2s_slc_queue are
// placed onto the list when they're filled by DMA

typedef struct slc_queue_item {
  uint32_t                blocksize : 12;
  uint32_t                datalen   : 12;
  uint32_t                unused    :  5;
  uint32_t                sub_sof   :  1;
  uint32_t                eof       :  1;
  volatile uint32_t       owner     :  1; // DMA can change this value
  uint32_t *              buf_ptr;
  struct slc_queue_item * next_link_ptr;
} slc_queue_item_t;

typedef struct i2s_state {
  uint32_t *       slc_queue[SLC_BUF_CNT];
  volatile uint8_t slc_queue_len;
  uint32_t *       slc_buf_pntr[SLC_BUF_CNT]; // Pointer to the I2S DMA buffer data
  slc_queue_item_t slc_items[SLC_BUF_CNT]; // I2S DMA buffer descriptors
  uint32_t *       curr_slc_buf; // Current buffer for writing
  uint32_t         curr_slc_buf_pos; // Position in the current buffer
  void             (*callback) (void);
  // Callback function should be defined as 'void ICACHE_RAM_ATTR function_name()',
  // and be placed in IRAM for faster execution. Avoid long computational tasks in this
  // function, use it to set flags and process later.
  bool             driveClocks;
} i2s_state_t;

// RX = I2S receive (i.e. microphone), TX = I2S transmit (i.e. DAC)
static i2s_state_t *rx = NULL;
static i2s_state_t *tx = NULL;

// Last I2S sample rate requested
static uint32_t _i2s_sample_rate;

// IOs used for I2S. Not defined in i2s.h, unfortunately.
// Note these are internal GPIO numbers and not pins on an
// Arduino board. Users need to verify their particular wiring.
#define I2SO_DATA 3
#define I2SO_BCK  15
#define I2SO_WS   2
#define I2SI_DATA 12
#define I2SI_BCK  13
#define I2SI_WS   14

static bool _i2s_is_full(const i2s_state_t *ch) {
  if (!ch) {
    return false;
  }
  return (ch->curr_slc_buf_pos==SLC_BUF_LEN || ch->curr_slc_buf==NULL) && (ch->slc_queue_len == 0);
}

bool i2s_is_full() {
  return _i2s_is_full( tx );
}

bool i2s_rx_is_full() {
  return _i2s_is_full( rx );
}

static bool _i2s_is_empty(const i2s_state_t *ch) {
  if (!ch) {
    return false;
  }
  return (ch->slc_queue_len >= SLC_BUF_CNT-1);
}

bool i2s_is_empty() {
  return _i2s_is_empty( tx );
}

bool i2s_rx_is_empty() {
  return _i2s_is_empty( rx );
}

static uint16_t _i2s_available(const i2s_state_t *ch) {
  if (!ch) {
    return 0;
  }
  return (SLC_BUF_CNT - ch->slc_queue_len) * SLC_BUF_LEN;
}

uint16_t i2s_available(){
  return _i2s_available( tx );
}

uint16_t i2s_rx_available(){
  return _i2s_available( rx );
}

// Pop the top off of the queue and return it
static uint32_t * ICACHE_RAM_ATTR i2s_slc_queue_next_item(i2s_state_t *ch) {
  uint8_t i;
  uint32_t *item = ch->slc_queue[0];
  ch->slc_queue_len--;
  for ( i = 0; i < ch->slc_queue_len; i++) {
    ch->slc_queue[i] = ch->slc_queue[i+1];
  }
  return item;
}

// Append an item to the end of the queue from receive
static void ICACHE_RAM_ATTR i2s_slc_queue_append_item(i2s_state_t *ch, uint32_t *item) {
  // Shift everything up, except for the one corresponding to this item
  for (int i=0, dest=0; i < ch->slc_queue_len; i++) {
    if (ch->slc_queue[i] != item) {
      ch->slc_queue[dest++] = ch->slc_queue[i];
    }
  }
  if (ch->slc_queue_len < SLC_BUF_CNT - 1) {
    ch->slc_queue[ch->slc_queue_len++] = item;
  } else {
    ch->slc_queue[ch->slc_queue_len] = item;
  }
}

static void ICACHE_RAM_ATTR i2s_slc_isr(void) {
  ETS_SLC_INTR_DISABLE();
  uint32_t slc_intr_status = SLCIS;
  SLCIC = 0xFFFFFFFF;
  if (slc_intr_status & SLCIRXEOF) {
    slc_queue_item_t *finished_item = (slc_queue_item_t *)SLCRXEDA;
    // Zero the buffer so it is mute in case of underflow
    ets_memset((void *)finished_item->buf_ptr, 0x00, SLC_BUF_LEN * 4);
    if (tx->slc_queue_len >= SLC_BUF_CNT-1) {
      // All buffers are empty. This means we have an underflow
      i2s_slc_queue_next_item(tx); // Free space for finished_item
    }
    tx->slc_queue[tx->slc_queue_len++] = finished_item->buf_ptr;
    if (tx->callback) {
      tx->callback();
    }
  }
  if (slc_intr_status & SLCITXEOF) {
    slc_queue_item_t *finished_item = (slc_queue_item_t *)SLCTXEDA;
    // Set owner back to 1 (SW) or else RX stops.  TX has no such restriction.
    finished_item->owner = 1;
    i2s_slc_queue_append_item(rx, finished_item->buf_ptr);
    if (rx->callback) {
      rx->callback();
    }
  }
  ETS_SLC_INTR_ENABLE();
}

void i2s_set_callback(void (*callback) (void)) {
  tx->callback = callback;
}

void i2s_rx_set_callback(void (*callback) (void)) {
  rx->callback = callback;
}

static bool _alloc_channel(i2s_state_t *ch) {
  ch->slc_queue_len = 0;
  for (int x=0; x<SLC_BUF_CNT; x++) {
    ch->slc_buf_pntr[x] = (uint32_t *)malloc(SLC_BUF_LEN * sizeof(ch->slc_buf_pntr[0][0]));
    if (!ch->slc_buf_pntr[x]) {
      // OOM, the upper layer will free up any partially allocated channels.
      return false;
    }
    memset(ch->slc_buf_pntr[x], 0, SLC_BUF_LEN * sizeof(ch->slc_buf_pntr[x][0]));

    ch->slc_items[x].unused = 0;
    ch->slc_items[x].owner = 1;
    ch->slc_items[x].eof = 1;
    ch->slc_items[x].sub_sof = 0;
    ch->slc_items[x].datalen = SLC_BUF_LEN * 4;
    ch->slc_items[x].blocksize = SLC_BUF_LEN * 4;
    ch->slc_items[x].buf_ptr = (uint32_t*)&ch->slc_buf_pntr[x][0];
    ch->slc_items[x].next_link_ptr = (x<(SLC_BUF_CNT-1))?(&ch->slc_items[x+1]):(&ch->slc_items[0]);
  }
  return true;
}

static bool i2s_slc_begin() {
  if (tx) {
    if (!_alloc_channel(tx)) {
      return false;
    }
  }
  if (rx) {
    if (!_alloc_channel(rx)) {
      return false;
    }
  }

  ETS_SLC_INTR_DISABLE();
  SLCC0 |= SLCRXLR | SLCTXLR;
  SLCC0 &= ~(SLCRXLR | SLCTXLR);
  SLCIC = 0xFFFFFFFF;

  // Configure DMA
  SLCC0 &= ~(SLCMM << SLCM); // Clear DMA MODE
  SLCC0 |= (1 << SLCM); // Set DMA MODE to 1
  SLCRXDC |= SLCBINR | SLCBTNR; // Enable INFOR_NO_REPLACE and TOKEN_NO_REPLACE
  SLCRXDC &= ~(SLCBRXFE | SLCBRXEM | SLCBRXFM); // Disable RX_FILL, RX_EOF_MODE and RX_FILL_MODE

  //Feed DMA the 1st buffer desc addr
  //To send data to the I2S subsystem, counter-intuitively we use the RXLINK part, not the TXLINK as you might
  //expect. The TXLINK part still needs a valid DMA descriptor, even if it's unused: the DMA engine will throw
  //an error at us otherwise. Just feed it any random descriptor.
  SLCTXL &= ~(SLCTXLAM << SLCTXLA); // clear TX descriptor address
  SLCRXL &= ~(SLCRXLAM << SLCRXLA); // clear RX descriptor address
  if (!rx) {
    SLCTXL |= (uint32)&tx->slc_items[1] << SLCTXLA; // Set fake (unused) RX descriptor address
  } else {
    SLCTXL |= (uint32)&rx->slc_items[0] << SLCTXLA; // Set real RX address
  }
  if (!tx) {
    SLCRXL |= (uint32)&rx->slc_items[1] << SLCRXLA; // Set fake (unused) TX descriptor address
  } else {
    SLCRXL |= (uint32)&tx->slc_items[0] << SLCRXLA; // Set real TX address
  }

  ETS_SLC_INTR_ATTACH(i2s_slc_isr, NULL);
  SLCIE = (tx?SLCIRXEOF:0) | (rx?SLCITXEOF:0); // Enable appropriate EOF IRQ

  ETS_SLC_INTR_ENABLE();

  // Start transmission ("TX" DMA always needed to be enabled)
  SLCTXL |= SLCTXLS;
  if (tx) {
    SLCRXL |= SLCRXLS;
  }

  return true;
}

static void i2s_slc_end(){
  ETS_SLC_INTR_DISABLE();
  SLCIC = 0xFFFFFFFF;
  SLCIE = 0;
  SLCTXL &= ~(SLCTXLAM << SLCTXLA); // clear TX descriptor address
  SLCRXL &= ~(SLCRXLAM << SLCRXLA); // clear RX descriptor address

  for (int x = 0; x<SLC_BUF_CNT; x++) {
    if (tx) {
      free(tx->slc_buf_pntr[x]);
      tx->slc_buf_pntr[x] = NULL;
    }
    if (rx) {
      free(rx->slc_buf_pntr[x]);
      rx->slc_buf_pntr[x] = NULL;
    }
  }
}

// These routines push a single, 32-bit sample to the I2S buffers. Call at (on average)
// at least the current sample rate.
static bool _i2s_write_sample(uint32_t sample, bool nb) {
  if (!tx) {
    return false;
  }

  if (tx->curr_slc_buf_pos==SLC_BUF_LEN || tx->curr_slc_buf==NULL) {
    if (tx->slc_queue_len == 0) {
      if (nb) {
        // Don't wait if nonblocking, just notify upper levels
        return false;
      }
      while (1) {
        if (tx->slc_queue_len > 0) {
          break;
        } else {
          optimistic_yield(10000);
        }
      }
    }
    ETS_SLC_INTR_DISABLE();
    tx->curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item(tx);
    ETS_SLC_INTR_ENABLE();
    tx->curr_slc_buf_pos=0;
  }
  tx->curr_slc_buf[tx->curr_slc_buf_pos++]=sample;
  return true;
}

bool i2s_write_sample(uint32_t sample) {
  return _i2s_write_sample(sample, false);
}

bool i2s_write_sample_nb(uint32_t sample) {
  return _i2s_write_sample(sample, true);
}

bool i2s_write_lr(int16_t left, int16_t right){
  int sample = right & 0xFFFF;
  sample = sample << 16;
  sample |= left & 0xFFFF;
  return i2s_write_sample(sample);
}

// writes a buffer of frames into the DMA memory, returns the amount of frames written
// A frame is just a int16_t for mono, for stereo a frame is two int16_t, one for each channel.
static uint16_t _i2s_write_buffer(int16_t *frames, uint16_t frame_count, bool mono, bool nb) {
    uint16_t frames_written=0;

    while(frame_count>0) {
   
        // make sure we have room in the current buffer
        if (tx->curr_slc_buf_pos==SLC_BUF_LEN || tx->curr_slc_buf==NULL) {
            // no room in the current buffer? if there are no buffers available then exit
            if (tx->slc_queue_len == 0)
            {
                if (nb) {
                    // if nonblocking just return the number of frames written so far
                    break;
                }
                else {
                    while (1) {
                        if (tx->slc_queue_len > 0) {
                          break;
                        } else {
                          optimistic_yield(10000);
                        }
                    }
                }
            }
            
            // get a new buffer
            ETS_SLC_INTR_DISABLE();
            tx->curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item(tx);
            ETS_SLC_INTR_ENABLE();
            tx->curr_slc_buf_pos=0;
        }       

        //space available in the current buffer
        uint16_t	available = SLC_BUF_LEN - tx->curr_slc_buf_pos;

        uint16_t fc = (available < frame_count) ? available : frame_count;

        if (mono) {
            for(uint16_t i=0;i<fc;i++){
                uint16_t v = (uint16_t)(*frames++);
                tx->curr_slc_buf[tx->curr_slc_buf_pos++] = (v << 16) | v;               
            }                
        }
        else
        {        
            for(uint16_t i=0;i<fc;i++){
                uint16_t v1 = (uint16_t)(*frames++);
                uint16_t v2 = (uint16_t)(*frames++);
                tx->curr_slc_buf[tx->curr_slc_buf_pos++] = (v1 << 16) | v2;
            }
        }        
        
        frame_count -= fc;
        frames_written += fc;
    }
    return frames_written;
}

uint16_t i2s_write_buffer_mono_nb(int16_t *frames, uint16_t frame_count) { return _i2s_write_buffer(frames, frame_count, true, true); }

uint16_t i2s_write_buffer_mono(int16_t *frames, uint16_t frame_count) { return _i2s_write_buffer(frames, frame_count, true, false); }

uint16_t i2s_write_buffer_nb(int16_t *frames, uint16_t frame_count) { return _i2s_write_buffer(frames, frame_count, false, true); }

uint16_t i2s_write_buffer(int16_t *frames, uint16_t frame_count) { return _i2s_write_buffer(frames, frame_count, false, false); }

bool i2s_read_sample(int16_t *left, int16_t *right, bool blocking) {
  if (!rx) {
    return false;
  }
  if (rx->curr_slc_buf_pos==SLC_BUF_LEN || rx->curr_slc_buf==NULL) {
    if (rx->slc_queue_len == 0) {
      if (!blocking) {
        return false;
      }
      while (1) {
        if (rx->slc_queue_len > 0){
          break;
        } else {
          optimistic_yield(10000);
        }
      }
    }
    ETS_SLC_INTR_DISABLE();
    rx->curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item(rx);
    ETS_SLC_INTR_ENABLE();
    rx->curr_slc_buf_pos=0;
  }

  uint32_t sample = rx->curr_slc_buf[rx->curr_slc_buf_pos++];
  if (left) {
    *left  = sample & 0xffff;
  }
  if (right) {
    *right = sample >> 16;
  }

  return true;
}


void i2s_set_rate(uint32_t rate) { //Rate in HZ
  if (rate == _i2s_sample_rate) {
    return;
  }
  _i2s_sample_rate = rate;

  uint32_t scaled_base_freq = I2SBASEFREQ/32;
  float delta_best = scaled_base_freq;

  uint8_t sbd_div_best=1;
  uint8_t scd_div_best=1;
  for (uint8_t i=1; i<64; i++) {
    for (uint8_t j=i; j<64; j++) {
      float new_delta = fabs(((float)scaled_base_freq/i/j) - rate);
      if (new_delta < delta_best){
      	delta_best = new_delta;
        sbd_div_best = i;
        scd_div_best = j;
      }
    }
  }

  i2s_set_dividers( sbd_div_best, scd_div_best );
}

void i2s_set_dividers(uint8_t div1, uint8_t div2) {
  // Ensure dividers fit in bit fields
  div1 &= I2SBDM;
  div2 &= I2SCDM;

  /*
  Following this post: https://github.com/esp8266/Arduino/issues/2590
  We should reset the transmitter while changing the configuration bits to avoid random distortion.
  */
 
  uint32_t i2sc_temp = I2SC;
  i2sc_temp |= (I2STXR); // Hold transmitter in reset
  I2SC = i2sc_temp;

  // trans master(active low), recv master(active_low), !bits mod(==16 bits/chanel), clear clock dividers
  i2sc_temp &= ~(I2STSM | I2SRSM | (I2SBMM << I2SBM) | (I2SBDM << I2SBD) | (I2SCDM << I2SCD));

  // I2SRF = Send/recv right channel first (? may be swapped form I2S spec of WS=0 => left)
  // I2SMR = MSB recv/xmit first
  // I2SRMS, I2STMS = 1-bit delay from WS to MSB (I2S format)
  // div1, div2 = Set I2S WS clock frequency.  BCLK seems to be generated from 32x this
  i2sc_temp |= I2SRF | I2SMR | I2SRMS | I2STMS | (div1 << I2SBD) | (div2 << I2SCD);

  I2SC = i2sc_temp;
  
  i2sc_temp &= ~(I2STXR); // Release reset
  I2SC = i2sc_temp;
}

float i2s_get_real_rate(){
  return (float)I2SBASEFREQ/32/((I2SC>>I2SBD) & I2SBDM)/((I2SC >> I2SCD) & I2SCDM);
}

bool i2s_rxtx_begin(bool enableRx, bool enableTx) {
  return i2s_rxtxdrive_begin(enableRx, enableTx, true, true);
}

bool i2s_rxtxdrive_begin(bool enableRx, bool enableTx, bool driveRxClocks, bool driveTxClocks) {
  if (tx || rx) {
    i2s_end(); // Stop and free any ongoing stuff
  }

  if (enableTx) {
    tx = (i2s_state_t*)calloc(1, sizeof(*tx));
    if (!tx) {
      // Nothing to clean up yet
      return false; // OOM Error!
    }
    tx->driveClocks = driveTxClocks;
    pinMode(I2SO_DATA, FUNCTION_1);
    if (driveTxClocks) {
      pinMode(I2SO_WS, FUNCTION_1);
      pinMode(I2SO_BCK, FUNCTION_1);
    }
  }
  if (enableRx) {
    rx = (i2s_state_t*)calloc(1, sizeof(*rx));
    if (!rx) {
      i2s_end(); // Clean up any TX or pin changes
      return false; // OOM error!
    }
    rx->driveClocks = driveRxClocks;
    pinMode(I2SI_DATA, INPUT);
    if (driveRxClocks) {
      pinMode(I2SI_WS, OUTPUT);
      pinMode(I2SI_BCK, OUTPUT);
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_I2SI_BCK);
      PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_I2SI_WS);
    }
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_I2SI_DATA);
  }

  if (!i2s_slc_begin()) {
    // OOM in SLC memory allocations, tear it all down and abort!
    i2s_end();
    return false;
  }
  
  I2S_CLK_ENABLE();
  I2SIC = 0x3F;
  I2SIE = 0;
  
  // Reset I2S
  I2SC &= ~(I2SRST);
  I2SC |= I2SRST;
  I2SC &= ~(I2SRST);
  
  // I2STXFMM, I2SRXFMM=0 => 16-bit, dual channel data shifted in/out
  I2SFC &= ~(I2SDE | (I2STXFMM << I2STXFM) | (I2SRXFMM << I2SRXFM)); // Set RX/TX FIFO_MOD=0 and disable DMA (FIFO only)
  I2SFC |= I2SDE; // Enable DMA

  // I2STXCMM, I2SRXCMM=0 => Dual channel mode
  I2SCC &= ~((I2STXCMM << I2STXCM) | (I2SRXCMM << I2SRXCM)); // Set RX/TX CHAN_MOD=0

  // Ensure a sane clock is set, but don't change any pre-existing ones.
  // But we also need to make sure the other bits weren't reset by a previous
  // reset.  So, store the present one, clear the flag, then set the same
  // value (writing all needed config bits in the process
  uint32_t save_rate = _i2s_sample_rate;
  _i2s_sample_rate  = 0;
   i2s_set_rate(save_rate ? save_rate : 44100);

  if (rx) {
    // Need to prime the # of samples to receive in the engine
    I2SRXEN = SLC_BUF_LEN;
  }

  I2SC |= (rx?I2SRXS:0) | (tx?I2STXS:0); // Start transmission/reception

  return true;
}

void i2s_begin() {
  i2s_rxtx_begin(false, true);
}

void i2s_end() {
  // Disable any I2S send or receive
  // ? Maybe not needed since we're resetting on the next line...
  I2SC &= ~(I2STXS | I2SRXS);

  // Reset I2S
  I2SC &= ~(I2SRST);
  I2SC |= I2SRST;
  I2SC &= ~(I2SRST);

  i2s_slc_end();

  if (tx) {
    pinMode(I2SO_DATA, INPUT);
    if (tx->driveClocks) {
      pinMode(I2SO_BCK, INPUT);
      pinMode(I2SO_WS, INPUT);
    }
    free(tx);
    tx = NULL;
  }
  if (rx) {
    pinMode(I2SI_DATA, INPUT);
    if (rx->driveClocks) {
      pinMode(I2SI_BCK, INPUT);
      pinMode(I2SI_WS, INPUT);
    }
    free(rx);
    rx = NULL;
  }
}

};
