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

extern void ets_wdt_enable(void);
extern void ets_wdt_disable(void);

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
  uint32_t blocksize : 12;
  uint32_t datalen   : 12;
  uint32_t unused    :  5;
  uint32_t sub_sof   :  1;
  volatile uint32_t eof       :  1;
  volatile uint32_t owner     :  1;
  uint32_t *buf_ptr;
  uint32_t *next_link_ptr;
  // This is my own way of tracking item ID
  volatile uint32_t myid;
} slc_queue_item_t;

typedef struct i2s_state {
  uint32_t *       i2s_slc_queue[SLC_BUF_CNT];
  volatile uint8_t i2s_slc_queue_len;
  uint32_t *       i2s_slc_buf_pntr[SLC_BUF_CNT]; // Pointer to the I2S DMA buffer data
  slc_queue_item_t i2s_slc_items[SLC_BUF_CNT]; // I2S DMA buffer descriptors
  uint32_t *       i2s_curr_slc_buf; // Current buffer for writing
  uint32_t         i2s_curr_slc_buf_pos; // Position in the current buffer
  void             (*i2s_callback) (void);
  // Callback function should be defined as 'void ICACHE_FLASH_ATTR function_name()',
  // and be placed in IRAM for faster execution. Avoid long computational tasks in this
  // function, use it to set flags and process later.
} i2s_state_t;

// RX = I2S receive (i.e. microphone), TX = I2S transmit (i.e. DAC)
static i2s_state_t *rx = NULL;
static i2s_state_t *tx = NULL;

volatile int rx_irqs = 0;
volatile int tx_irqs = 0;

// Some constants that aren't defined in i2s_regs.h
#define I2SI_DATA 12
#define I2SI_BCK  13
#define I2SI_WS   14


static bool ICACHE_FLASH_ATTR _i2s_is_full(const i2s_state_t *ch) {
  if (!ch) {
    return false;
  } else {
    return (ch->i2s_curr_slc_buf_pos==SLC_BUF_LEN || ch->i2s_curr_slc_buf==NULL) && (ch->i2s_slc_queue_len == 0);
  }
}

bool ICACHE_FLASH_ATTR i2s_is_full() {
  return _i2s_is_full( tx );
}

bool ICACHE_FLASH_ATTR i2s_rx_is_full() {
  return _i2s_is_full( rx );
}

static bool ICACHE_FLASH_ATTR _i2s_is_empty(const i2s_state_t *ch) {
  if (!ch) {
    return false;
  } else {
    return (ch->i2s_slc_queue_len >= SLC_BUF_CNT-1);
  }
}

bool ICACHE_FLASH_ATTR i2s_is_empty() {
  return _i2s_is_empty( tx );
}

bool ICACHE_FLASH_ATTR i2s_rx_is_empty() {
  return _i2s_is_empty( rx );
}

static int16_t ICACHE_FLASH_ATTR _i2s_available(const i2s_state_t *ch) {
  if (!ch) {
    return 0;
  } else {
    return (SLC_BUF_CNT - ch->i2s_slc_queue_len) * SLC_BUF_LEN;
  }
}

int16_t ICACHE_FLASH_ATTR i2s_available(){
  return _i2s_available( tx );
}

int16_t ICACHE_FLASH_ATTR i2s_rx_available(){
  return _i2s_available( rx );
}

// Pop the top off of the queue and return it
uint32_t * ICACHE_FLASH_ATTR i2s_slc_queue_next_item(i2s_state_t *ch) {
  uint8_t i;
  uint32_t *item = ch->i2s_slc_queue[0];
  ch->i2s_slc_queue_len--;
  for ( i = 0; i < ch->i2s_slc_queue_len; i++) {
    ch->i2s_slc_queue[i] = ch->i2s_slc_queue[i+1];
  }
  return item;
}

// Append an item to the end of the queue from receive
void ICACHE_FLASH_ATTR i2s_slc_queue_append_item(i2s_state_t *ch, uint32_t *item) {
  // Shift everything up, except for the one corresponding to this item
  for (int i=0, dest=0; i < ch->i2s_slc_queue_len; i++) {
    if (ch->i2s_slc_queue[i] != item) {
      ch->i2s_slc_queue[dest++] = ch->i2s_slc_queue[i];
    }
  }
  if (ch->i2s_slc_queue_len < SLC_BUF_CNT - 1) {
    ch->i2s_slc_queue[ch->i2s_slc_queue_len++] = item;
  } else {
    ch->i2s_slc_queue[ch->i2s_slc_queue_len] = item;
  }
}

// This routine is called as soon as the DMA routine has something to tell us. All we
// handle here is the RX_EOF_INT status, which indicate the DMA has sent a buffer whose
// descriptor has the 'EOF' field set to 1.
void ICACHE_FLASH_ATTR i2s_slc_isr(void) {
  uint32_t slc_intr_status = SLCIS;
  SLCIC = 0xFFFFFFFF;
  if (slc_intr_status & SLCIRXEOF) {
    ETS_SLC_INTR_DISABLE();
    tx_irqs++;
    slc_queue_item_t *finished_item = (slc_queue_item_t *)SLCRXEDA;
    memset((void *)finished_item->buf_ptr, 0x00, SLC_BUF_LEN * 4);//zero the buffer so it is mute in case of underflow
    if (tx->i2s_slc_queue_len >= SLC_BUF_CNT-1) { //All buffers are empty. This means we have an underflow
      i2s_slc_queue_next_item(tx); //free space for finished_item
    }
    tx->i2s_slc_queue[tx->i2s_slc_queue_len++] = finished_item->buf_ptr;
    if (tx->i2s_callback) tx->i2s_callback();
    ETS_SLC_INTR_ENABLE();
  }
  if (slc_intr_status & SLCITXEOF) {
    ETS_SLC_INTR_DISABLE();
    rx_irqs++;
    slc_queue_item_t *finished_item = (slc_queue_item_t *)SLCTXEDA;
    finished_item->owner = 1; // Or else RX just stops
    finished_item->myid++;
    i2s_slc_queue_append_item(rx, finished_item->buf_ptr);
    if (rx->i2s_callback) rx->i2s_callback();
    ETS_SLC_INTR_ENABLE();
  }
}

void i2s_set_callback(void (*callback) (void)){
  tx->i2s_callback = callback;
}

void i2s_rx_set_callback(void (*callback) (void)){
  rx->i2s_callback = callback;
}

static void ICACHE_FLASH_ATTR _alloc_channel(i2s_state_t *ch) {
  int x, y;

  ch->i2s_slc_queue_len = 0;
  for (x=0; x<SLC_BUF_CNT; x++) {
    ch->i2s_slc_buf_pntr[x] = malloc(SLC_BUF_LEN*4);
    for (y=0; y<SLC_BUF_LEN; y++) ch->i2s_slc_buf_pntr[x][y] = 0;

    ch->i2s_slc_items[x].unused = 0;
    ch->i2s_slc_items[x].owner = 1;
    ch->i2s_slc_items[x].eof = 1;
    ch->i2s_slc_items[x].sub_sof = 0;
    ch->i2s_slc_items[x].datalen = SLC_BUF_LEN * 4;
    ch->i2s_slc_items[x].blocksize = SLC_BUF_LEN * 4;
    ch->i2s_slc_items[x].buf_ptr = (uint32_t*)&ch->i2s_slc_buf_pntr[x][0];
    ch->i2s_slc_items[x].next_link_ptr = (uint32_t*)((x<(SLC_BUF_CNT-1))?(&ch->i2s_slc_items[x+1]):(&ch->i2s_slc_items[0]));
    ch->i2s_slc_items[x].myid = 0;
  }
}
#if 0
void dumprx()
{
for (int i=0; i<SLC_BUF_CNT; i++) {
printf("%d: %d %d %d %d %d %d %p %p\n", i, rx->i2s_slc_items[i].myid, rx->i2s_slc_items[i].owner, rx->i2s_slc_items[i].eof, rx->i2s_slc_items[i].sub_sof, rx->i2s_slc_items[i].datalen, rx->i2s_slc_items[i].blocksize,
rx->i2s_slc_items[i].buf_ptr, rx->i2s_slc_items[i].next_link_ptr);
}
}
#endif


static void ICACHE_FLASH_ATTR i2s_slc_begin() {
  if (tx) {
    _alloc_channel(tx);
  }
  if (rx) {
    _alloc_channel(rx);
  }

  ETS_SLC_INTR_DISABLE();
  SLCC0 |= SLCRXLR | SLCTXLR;
  SLCC0 &= ~(SLCRXLR | SLCTXLR);
  SLCIC = 0xFFFFFFFF;

  // Configure DMA
  SLCC0 &= ~(SLCMM << SLCM); // Clear DMA MODE
  SLCC0 |= (1 << SLCM); // Set DMA MODE to 1
  SLCRXDC |= SLCBINR | SLCBTNR; // Enable INFOR_NO_REPLACE and TOKEN_NO_REPLACE
  SLCRXDC &= ~(/*SLCBRXFE |*/ SLCBRXEM | SLCBRXFM); //disable RX_FILL, RX_EOF_MODE and RX_FILL_MODE

  //Feed DMA the 1st buffer desc addr
  //To send data to the I2S subsystem, counter-intuitively we use the RXLINK part, not the TXLINK as you might
  //expect. The TXLINK part still needs a valid DMA descriptor, even if it's unused: the DMA engine will throw
  //an error at us otherwise. Just feed it any random descriptor.
  SLCTXL &= ~(SLCTXLAM << SLCTXLA); // clear TX descriptor address
  SLCRXL &= ~(SLCRXLAM << SLCRXLA); // clear RX descriptor address
  if (!rx) {
    SLCTXL |= (uint32)&tx->i2s_slc_items[1] << SLCTXLA; // Set fake (unused) RX descriptor address
  } else {
    SLCTXL |= (uint32)&rx->i2s_slc_items[0] << SLCTXLA; // Set real RX address
  }
  if (!tx) {
//    SLCRXL |= (uint32)&rx->i2s_slc_items[1] << SLCRXLA; // Set fake (ununsed) TX descriptor address
  } else {
    SLCRXL |= (uint32)&tx->i2s_slc_items[0] << SLCRXLA; // Set real TX address
  }

  ETS_SLC_INTR_ATTACH(i2s_slc_isr, NULL);
  SLCIE = (tx?SLCIRXEOF:0) | (rx?SLCITXEOF:0); // Enable appropriate EOF IRQ

  ETS_SLC_INTR_ENABLE();

  // Start transmission ("TX" DMA always needed to be enabled)
  SLCTXL |= SLCTXLS;
  if (tx) {
    SLCRXL |= SLCRXLS;
  }
}

static void ICACHE_FLASH_ATTR i2s_slc_end(){
  ETS_SLC_INTR_DISABLE();
  SLCIC = 0xFFFFFFFF;
  SLCIE = 0;
  SLCTXL &= ~(SLCTXLAM << SLCTXLA); // clear TX descriptor address
  SLCRXL &= ~(SLCRXLAM << SLCRXLA); // clear RX descriptor address

  for (int x = 0; x<SLC_BUF_CNT; x++) {
    if (tx) {
      free(tx->i2s_slc_buf_pntr[x]);
      tx->i2s_slc_buf_pntr[x] = NULL;
    }
    if (rx) {
      free(rx->i2s_slc_buf_pntr[x]);
      rx->i2s_slc_buf_pntr[x] = NULL;
    }
  }
}

//This routine pushes a single, 32-bit sample to the I2S buffers. Call this at (on average) 
//at least the current sample rate. You can also call it quicker: it will suspend the calling
//thread if the buffer is full and resume when there's room again.

static bool ICACHE_FLASH_ATTR _i2s_write_sample(uint32_t sample, bool nb) {
  if (tx->i2s_curr_slc_buf_pos==SLC_BUF_LEN || tx->i2s_curr_slc_buf==NULL) {
    if (tx->i2s_slc_queue_len == 0) {
      if (nb) return false;
      while (1) {
        if (tx->i2s_slc_queue_len > 0){
          break;
        } else {
          ets_wdt_disable();
          ets_wdt_enable();
        }
      }
    }
    ETS_SLC_INTR_DISABLE();
    tx->i2s_curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item(tx);
    ETS_SLC_INTR_ENABLE();
    tx->i2s_curr_slc_buf_pos=0;
  }
  tx->i2s_curr_slc_buf[tx->i2s_curr_slc_buf_pos++]=sample;
  return true;
}

bool ICACHE_FLASH_ATTR i2s_write_sample(uint32_t sample) {
  return _i2s_write_sample(sample, false);
}

bool ICACHE_FLASH_ATTR i2s_write_sample_nb(uint32_t sample) {
  return _i2s_write_sample(sample, true);
}

bool ICACHE_FLASH_ATTR i2s_write_lr(int16_t left, int16_t right){
  int sample = right & 0xFFFF;
  sample = sample << 16;
  sample |= left & 0xFFFF;
  return i2s_write_sample(sample);
}

bool ICACHE_FLASH_ATTR i2s_read_sample(uint32_t *left, uint32_t *right, bool blocking) {
  if (rx->i2s_curr_slc_buf_pos==SLC_BUF_LEN || rx->i2s_curr_slc_buf==NULL) {
    if (rx->i2s_slc_queue_len == 0) {
      if (!blocking) return false;
      while (1) {
        if (rx->i2s_slc_queue_len > 0){
          break;
        } else {
          ets_wdt_disable();
          ets_wdt_enable();
        }
      }
    }
    ETS_SLC_INTR_DISABLE();
    rx->i2s_curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item(rx);
    ETS_SLC_INTR_ENABLE();
    rx->i2s_curr_slc_buf_pos=0;
  }

//  *left  = rx->i2s_slc_items[0].buf_ptr[rx->i2s_curr_slc_buf_pos++];
//  *right = rx->i2s_slc_items[0].buf_ptr[rx->i2s_curr_slc_buf_pos++];
  *left  = rx->i2s_curr_slc_buf[rx->i2s_curr_slc_buf_pos++];
  *right = rx->i2s_curr_slc_buf[rx->i2s_curr_slc_buf_pos++];

  return true;
}

//  END DMA
// =========
// START I2S


static uint32_t _i2s_sample_rate;

void ICACHE_FLASH_ATTR i2s_set_rate(uint32_t rate){ //Rate in HZ
  if(rate == _i2s_sample_rate) return;
  _i2s_sample_rate = rate;

  uint32_t scaled_base_freq = I2SBASEFREQ/32;
  float delta_best = scaled_base_freq;

  uint8_t sbd_div_best=1;
  uint8_t scd_div_best=1;
  for (uint8_t i=1; i<64; i++){
    for (uint8_t j=i; j<64; j++){
      float new_delta = fabs(((float)scaled_base_freq/i/j) - rate);
      if (new_delta < delta_best){
      	delta_best = new_delta;
        sbd_div_best = i;
        scd_div_best = j;
      }
    }
  }

  //!trans master, !bits mod, rece slave mod, rece msb shift, right first, msb right
  I2SC &= ~(I2STSM | I2SRSM | /*(I2SBMM << I2SBM) |*/ (I2SBDM << I2SBD) | (I2SCDM << I2SCD));
  I2SC |= I2SRF | I2SMR | I2SRMS | ((sbd_div_best) << I2SBD) | ((scd_div_best) << I2SCD);
}

void ICACHE_FLASH_ATTR i2s_set_dividers(uint8_t div1, uint8_t div2){
  div1 &= I2SBDM;
  div2 &= I2SCDM;

  I2SC &= ~(I2STSM | I2SRSM | (I2SBMM << I2SBM) | (I2SBDM << I2SBD) | (I2SCDM << I2SCD));
  I2SC |= I2SRF | I2SMR | I2SRSM | I2SRMS | (div1 << I2SBD) | (div2 << I2SCD);
}

float ICACHE_FLASH_ATTR i2s_get_real_rate(){
  return (float)I2SBASEFREQ/32/((I2SC>>I2SBD) & I2SBDM)/((I2SC >> I2SCD) & I2SCDM);
}

void ICACHE_FLASH_ATTR i2s_rxtx_begin(bool enableRx, bool enableTx) {
  if (tx || rx) {
    i2s_end(); // Stop and free any ongoing stuff
  }

  if (enableTx) {
    tx = (i2s_state_t*)calloc(1, sizeof(*tx));
    if (!tx) {
      return; // OOM Error!
    }
    pinMode(2, FUNCTION_1);  // I2SO_WS (LRCK)
    pinMode(3, FUNCTION_1);  // I2SO_DATA (SDIN)
    pinMode(15, FUNCTION_1); // I2SO_BCK (SCLK)
  }
  if (enableRx) {
    rx = (i2s_state_t*)calloc(1, sizeof(*rx));
    if (!rx) {
      free(tx);
      tx = NULL;
      return; // OOM error!
    }
    pinMode(I2SI_WS, OUTPUT);
    pinMode(I2SI_BCK, OUTPUT);
    pinMode(I2SI_DATA, INPUT);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_I2SI_DATA);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_I2SI_BCK);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_I2SI_WS);
  }

  _i2s_sample_rate = 0;
  i2s_slc_begin();
  
  I2S_CLK_ENABLE();
  I2SIC = 0x3F;
  I2SIE = 0;
  
  // Reset I2S
  I2SC &= ~(I2SRST);
  I2SC |= I2SRST;
  I2SC &= ~(I2SRST);
  
  I2SFC &= ~(I2SDE | (I2STXFMM << I2STXFM) | (I2SRXFMM << I2SRXFM)); // Set RX/TX FIFO_MOD=0 and disable DMA (FIFO only)
  I2SFC |= I2SDE | (rx ? 2/*24bpc, 2ch*/<<I2SRXFM : 0); // Enable DMA, set RX format 24(32bits), 2 channels

  I2SCC &= ~((I2STXCMM << I2STXCM) | (I2SRXCMM << I2SRXCM)); // Set RX/TX CHAN_MOD=0

  i2s_set_rate(44100);

  if (rx) {
    I2SRXEN = SLC_BUF_LEN;
  }
//  I2SC |= (15<<I2SBM);

  I2SC |= (rx?I2SRXS:0) | (tx?I2STXS:0); // Start transmission/reception
}

void ICACHE_FLASH_ATTR i2s_begin() {
  i2s_rxtx_begin(false, true);
}

void ICACHE_FLASH_ATTR i2s_end() {
  I2SC &= ~I2STXS;

  //Reset I2S
  I2SC &= ~(I2SRST);
  I2SC |= I2SRST;
  I2SC &= ~(I2SRST);

  i2s_slc_end();

  if (tx) {
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(15, INPUT);
    free(tx);
    tx = NULL;
  }
  if (rx) {
    pinMode(I2SI_DATA, INPUT);
    pinMode(I2SI_BCK, INPUT);
    pinMode(I2SI_WS, INPUT);
    free(rx);
    rx = NULL;
  }
}
