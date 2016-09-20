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

//We use a queue to keep track of the DMA buffers that are empty. The ISR will push buffers to the back of the queue,
//the mp3 decode will pull them from the front and fill them. For ease, the queue will contain *pointers* to the DMA
//buffers, not the data itself. The queue depth is one smaller than the amount of buffers we have, because there's
//always a buffer that is being used by the DMA subsystem *right now* and we don't want to be able to write to that
//simultaneously.

struct slc_queue_item {
  uint32_t                blocksize:12;
  uint32_t                datalen:12;
  uint32_t                unused:5;
  uint32_t                sub_sof:1;
  uint32_t                eof:1;
  uint32_t                owner:1;
  uint32_t *              buf_ptr;
  struct slc_queue_item * next_link_ptr;
};

static size_t SLC_BUF_CNT = 0; //Number of buffers in the I2S circular buffer
static size_t SLC_BUF_LEN = 0; //Length of one buffer, in 32-bit words.

static uint32_t ** i2s_slc_queue;
static uint8_t i2s_slc_queue_len;
static uint32_t * i2s_slc_buf_pntr=NULL; // Pointer to the I2S DMA buffer data
static struct slc_queue_item * i2s_slc_items; //I2S DMA buffer descriptors
static uint32_t * i2s_curr_slc_buf=NULL;//current buffer for writing
static int i2s_curr_slc_buf_pos=0; //position in the current buffer

bool ICACHE_FLASH_ATTR i2s_is_full(){
  return (i2s_curr_slc_buf_pos==SLC_BUF_LEN || i2s_curr_slc_buf==NULL) && (i2s_slc_queue_len == 0);
}

bool ICACHE_FLASH_ATTR i2s_is_empty(){
  return (i2s_slc_queue_len >= SLC_BUF_CNT-1);
}

uint32_t * ICACHE_FLASH_ATTR i2s_slc_queue_next_item(){ //pop the top off the queue
  uint8_t i;
  uint32_t * item = i2s_slc_queue[0];
  i2s_slc_queue_len--;
  for(i=0;i<i2s_slc_queue_len;i++)
    i2s_slc_queue[i] = i2s_slc_queue[i+1];
  return item;
}

//This routine is called as soon as the DMA routine has something to tell us. All we
//handle here is the RX_EOF_INT status, which indicate the DMA has sent a buffer whose
//descriptor has the 'EOF' field set to 1.
void ICACHE_FLASH_ATTR i2s_slc_isr(void) {
  uint32_t slc_intr_status = SLCIS;
  SLCIC = 0xFFFFFFFF;
  if (slc_intr_status & SLCIRXEOF) {
    ETS_SLC_INTR_DISABLE();
    struct slc_queue_item *finished_item = (struct slc_queue_item*)SLCRXEDA;
    memset((void *)finished_item->buf_ptr, 0x00, SLC_BUF_LEN * 4);//zero the buffer so it is mute in case of underflow
    if (i2s_slc_queue_len >= SLC_BUF_CNT-1) { //All buffers are empty. This means we have an underflow
      i2s_slc_queue_next_item(); //free space for finished_item
    }
    i2s_slc_queue[i2s_slc_queue_len++] = finished_item->buf_ptr;
    ETS_SLC_INTR_ENABLE();
  }
}

void ICACHE_FLASH_ATTR i2s_init(size_t count, size_t length){
  SLC_BUF_CNT = count;
  SLC_BUF_LEN = length;

  const int nsamples = count * length;
  i2s_slc_buf_pntr = (uint32_t *)malloc(nsamples * sizeof(uint32_t));
  for (int i = 0; i < nsamples; ++i)
    i2s_slc_buf_pntr[i] = 0;
  i2s_slc_items = (struct slc_queue_item *)malloc(sizeof(*i2s_slc_items) * SLC_BUF_CNT);
  i2s_slc_queue = (uint32_t **)malloc(sizeof(uint32_t *) * (SLC_BUF_CNT-1));

  for (int i = 0;i < SLC_BUF_CNT;++i) {
      i2s_slc_items[i].unused = 0;
      i2s_slc_items[i].owner = 1;
      i2s_slc_items[i].eof = 1;
      i2s_slc_items[i].sub_sof = 0;
      i2s_slc_items[i].datalen = SLC_BUF_LEN * 4;
      i2s_slc_items[i].blocksize = SLC_BUF_LEN * 4;
      i2s_slc_items[i].buf_ptr = i2s_slc_buf_pntr + i * SLC_BUF_LEN;
      i2s_slc_items[i].next_link_ptr = i < (SLC_BUF_CNT - 1) ?
        &i2s_slc_items[i + 1] :
        &i2s_slc_items[0];
  }
}

void ICACHE_FLASH_ATTR i2s_deinit(){
  free(i2s_slc_buf_pntr);
  i2s_slc_buf_pntr = NULL;
  free(i2s_slc_items);
  i2s_slc_items = NULL;
  free(i2s_slc_queue);
  i2s_slc_queue = NULL;
}

void ICACHE_FLASH_ATTR i2s_slc_begin(){
  i2s_slc_queue_len = 0;

  ETS_SLC_INTR_DISABLE();
  SLCC0 |= SLCRXLR | SLCTXLR;
  SLCC0 &= ~(SLCRXLR | SLCTXLR);
  SLCIC = 0xFFFFFFFF;

  //Configure DMA
  SLCC0 &= ~(SLCMM << SLCM); //clear DMA MODE
  SLCC0 |= (1 << SLCM); //set DMA MODE to 1
  SLCRXDC |= SLCBINR | SLCBTNR; //enable INFOR_NO_REPLACE and TOKEN_NO_REPLACE
  SLCRXDC &= ~(SLCBRXFE | SLCBRXEM | SLCBRXFM); //disable RX_FILL, RX_EOF_MODE and RX_FILL_MODE

  //Feed DMA the 1st buffer desc addr
  //To send data to the I2S subsystem, counter-intuitively we use the RXLINK part, not the TXLINK as you might
  //expect. The TXLINK part still needs a valid DMA descriptor, even if it's unused: the DMA engine will throw
  //an error at us otherwise. Just feed it any random descriptor.
  SLCTXL &= ~(SLCTXLAM << SLCTXLA); // clear TX descriptor address
  SLCTXL |= (uint32)&i2s_slc_items[1] << SLCTXLA; //set TX descriptor address. any random desc is OK, we don't use TX but it needs to be valid
  SLCRXL &= ~(SLCRXLAM << SLCRXLA); // clear RX descriptor address
  SLCRXL |= (uint32)&i2s_slc_items[0] << SLCRXLA; //set RX descriptor address

  ETS_SLC_INTR_ATTACH(i2s_slc_isr, NULL);
  SLCIE = SLCIRXEOF; //Enable only for RX EOF interrupt

  ETS_SLC_INTR_ENABLE();

  //Start transmission
  SLCTXL |= SLCTXLS;
  SLCRXL |= SLCRXLS;
}

void ICACHE_FLASH_ATTR i2s_slc_end(){
  ETS_SLC_INTR_DISABLE();
  SLCIC = 0xFFFFFFFF;
  SLCIE = 0;
  SLCTXL &= ~(SLCTXLAM << SLCTXLA); // clear TX descriptor address
  SLCRXL &= ~(SLCRXLAM << SLCRXLA); // clear RX descriptor address
}

uint32_t * i2s_get_buffer() {
  if (i2s_curr_slc_buf_pos==SLC_BUF_LEN || i2s_curr_slc_buf==NULL) {
    if(i2s_slc_queue_len == 0){
      return NULL;
    }
    ETS_SLC_INTR_DISABLE();
    i2s_curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item();
    ETS_SLC_INTR_ENABLE();
    i2s_curr_slc_buf_pos=0;
  }
  return i2s_curr_slc_buf;
}

void i2s_put_buffer() {
  i2s_curr_slc_buf_pos=SLC_BUF_LEN;
}

//This routine pushes a single, 32-bit sample to the I2S buffers. Call this at (on average)
//at least the current sample rate. You can also call it quicker: it will suspend the calling
//thread if the buffer is full and resume when there's room again.

bool ICACHE_FLASH_ATTR i2s_write_sample(uint32_t sample) {
  if (i2s_curr_slc_buf_pos==SLC_BUF_LEN || i2s_curr_slc_buf==NULL) {
    if(i2s_slc_queue_len == 0){
      while(1){
        if(i2s_slc_queue_len > 0){
          break;
        } else {
          ets_wdt_disable();
          ets_wdt_enable();
        }
      }
    }
    ETS_SLC_INTR_DISABLE();
    i2s_curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item();
    ETS_SLC_INTR_ENABLE();
    i2s_curr_slc_buf_pos=0;
  }
  i2s_curr_slc_buf[i2s_curr_slc_buf_pos++]=sample;
  return true;
}

bool ICACHE_FLASH_ATTR i2s_write_sample_nb(uint32_t sample) {
  if (i2s_curr_slc_buf_pos==SLC_BUF_LEN || i2s_curr_slc_buf==NULL) {
    if(i2s_slc_queue_len == 0){
      return false;
    }
    ETS_SLC_INTR_DISABLE();
    i2s_curr_slc_buf = (uint32_t *)i2s_slc_queue_next_item();
    ETS_SLC_INTR_ENABLE();
    i2s_curr_slc_buf_pos=0;
  }
  i2s_curr_slc_buf[i2s_curr_slc_buf_pos++]=sample;
  return true;
}

bool ICACHE_FLASH_ATTR i2s_write_lr(int16_t left, int16_t right){
  uint32_t sample = right & 0xFFFF;
  sample = sample << 16;
  sample |= left & 0xFFFF;
  return i2s_write_sample(sample);
}

bool ICACHE_FLASH_ATTR i2s_write_lr_nb(int16_t left, int16_t right){
  uint32_t sample = right & 0xFFFF;
  sample = sample << 16;
  sample |= left & 0xFFFF;
  return i2s_write_sample_nb(sample);
}

//  END DMA
// =========
// START I2S


static uint32_t _i2s_sample_rate = 0;

void ICACHE_FLASH_ATTR i2s_set_rate(uint32_t rate){ //Rate in HZ
  if(rate == _i2s_sample_rate) return;
  _i2s_sample_rate = rate;

  // Find closest divider
  uint32_t basefreq = 160000000L;
  int bestfreq = 0;
  uint32_t i2s_clkm_div, i2s_bck_div;

  // CLK_I2S = CPU_FREQ / I2S_CLKM_DIV_NUM
  // BCLK = CLK_I2S / I2S_BCK_DIV_NUM
  // WS = BCLK/ 2 / (16 + I2S_BITS_MOD)
  // Note that I2S_CLKM_DIV_NUM must be >5 for I2S data
  // I2S_CLKM_DIV_NUM - 5 - 63
  // I2S_BCK_DIV_NUM - 2 - 63
  for (int bckdiv = 2; bckdiv < 64; bckdiv++) {
    for (int clkmdiv = 5; clkmdiv < 64; clkmdiv++) {
      uint32_t testfreq = basefreq / (bckdiv * clkmdiv * 32);
      if (abs(_i2s_sample_rate - testfreq) < abs(_i2s_sample_rate - bestfreq)) {
        bestfreq = testfreq;
        i2s_clkm_div = clkmdiv;
        i2s_bck_div = bckdiv;
      }
    }
  }

  // Apply the sample rate
  // ~I2S_TRANS_SLAVE_MOD (TX master mode)
  // ~I2S_BITS_MOD
  //  I2S_RIGHT_FIRST
  //  I2S_MSB_RIGHT
  //  I2S_RECE_SLAVE_MOD (RX slave mode)
  //  I2S_RECE_MSB_SHIFT (??)
  //  I2S_TRANS_MSB_SHIFT (??)

  // !trans master, !bits mod,
  // rece slave mod, rece msb shift, right first, msb right
  I2SC &= ~( I2STSM |           // TX master mode
             I2STMS |           // TX LSB first
            (I2SBMM << I2SBM) | // clear bits mode
            (I2SBDM << I2SBD) | // clear bck_div
            (I2SCDM << I2SCD)); // clear clkm_div
  I2SC |= I2SRF |               // right first
          I2SMR |               // MSB first
          I2SRSM |              // RX slave mode
          I2SRMS |              // receive MSB shift
                                // bits_mode == 0 (16bit)
          ((i2s_bck_div & I2SBDM) << I2SBD) | // set bck_div
          ((i2s_clkm_div & I2SCDM) << I2SCD); // set clkm_div
}

void ICACHE_FLASH_ATTR i2s_begin(){
  i2s_slc_begin();

  pinMode(2, FUNCTION_1); //I2SO_WS (LRCK)
  pinMode(3, FUNCTION_1); //I2SO_DATA (SDIN)
  pinMode(15, FUNCTION_1); //I2SO_BCK (SCLK)

  I2S_CLK_ENABLE();
  I2SIC = 0x3F;
  I2SIE = 0;

  //Reset I2S
  I2SC &= ~(I2SRST);
  I2SC |= I2SRST;
  I2SC &= ~(I2SRST);

  I2SFC &= ~(I2SDE | (I2STXFMM << I2STXFM) | (I2SRXFMM << I2SRXFM)); //Set RX/TX FIFO_MOD=0 and disable DMA (FIFO only)
  I2SFC |= I2SDE; //Enable DMA
  I2SCC &= ~((I2STXCMM << I2STXCM) | (I2SRXCMM << I2SRXCM)); //Set RX/TX CHAN_MOD=0

  // defaults to 44100 if unset
  i2s_set_rate(_i2s_sample_rate == 0 ? 44100 : _i2s_sample_rate);

  I2SC |= I2STXS; //Start transmission
}

void ICACHE_FLASH_ATTR i2s_end(){
  i2s_slc_end();
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(15, INPUT);
}
