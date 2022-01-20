/*
  Overview: Of the 48KB of IRAM, the remaining IRAM after your code is untouched
  during the reboot process. For a sketch that does not use deep-sleep, it is
  possible to pass/hold information across boot cycles in this area of IRAM.

  With the selection of Arduino IDE Tools Option: 'MMU: 16KB cache + 48KB IRAM
  and 2nd Heap (shared)' all of this space goes into a managed 2nd Heap.
  Managed, in this case, refers to using malloc, free, realloc, etc. API.

  The objective of this example is to show how to modify the 2nd Heap creation
  to omit a block of IRAM at the end of the 2nd Heap. In this example, we use
  this block to store a boot count.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <umm_malloc/umm_malloc.h>
#if defined(UMM_HEAP_IRAM)

#if defined(CORE_MOCK)
#define XCHAL_INSTRAM1_VADDR 0x40100000
#else
#include <sys/config.h>  // For config/core-isa.h
#endif

// durable - as in long life, persisting across reboots.
struct durable
{
  uint32_t bootCounter;
  uint32_t chksum;
};

// Leave a durable block of IRAM after the 2nd heap.

// The block should be in 8-byte increments and fall on an 8-byte alignment.
#define IRAM_RESERVE_SZ ((sizeof(struct durable) + 7UL) & ~7UL)

// Position its address just above the reduced 2nd Heap.
#define IRAM_RESERVE ((uintptr_t)XCHAL_INSTRAM1_VADDR + 0xC000UL - IRAM_RESERVE_SZ)

// Define a reference with the right properties to make access easier.
#define DURABLE ((struct durable*)IRAM_RESERVE)
#define INCREMENT_BOOTCOUNT() (DURABLE->bootCounter)++

extern struct rst_info resetInfo;

/*
  Define a function to determine if IRAM stored data is valid. The criteria used
  here can vary with how exhaustively you want the process to be.

  In this example, we are just going to look at the reset cause and assume all
  is well in certain situations. For this example, we include
  REASON_EXT_SYS_RST as a possible case for IRAM not being valid. The problem
  here is some devices will indicate REASON_EXT_SYS_RST for the Power-on case.

  If you wanted to be able to isolate the power-on case from a
  REASON_EXT_SYS_RST, you could add additional logic to set and verify a CRC or
  XOR sum on the IRAM data (or just a section of the IRAM data).
*/
inline bool            is_iram_valid(void)
{
  return (REASON_WDT_RST <= resetInfo.reason && REASON_SOFT_RESTART >= resetInfo.reason);
}

void setup()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(10);
  Serial.printf_P(PSTR("\r\nSetup ...\r\n"));

  if (!is_iram_valid())
  {
    DURABLE->bootCounter = 0;
  }

  DURABLE->bootCounter++;

  Serial.printf("Number of reboots at %u\r\n", DURABLE->bootCounter);
  Serial.printf("\r\nSome less than direct, ways to restart:\r\n");
  processKey(Serial, '?');
}

void loop(void)
{
  if (Serial.available() > 0)
  {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }
}

//////////////////////////////////////////////////////////////////////////////

/*
  Create a block of unmanaged IRAM for special uses.

  This is done by reducing the size of the managed 2nd Heap (Shared) at
  initialization time.
*/

extern "C" void _text_end(void);

extern "C" void umm_init_iram(void)
{
  /*
    Calculate the start of 2nd heap, staying clear of possible segment alignment
    adjustments and checksums. These can affect the persistence of data across
    reboots.
  */
  uintptr_t sec_heap = (uintptr_t)_text_end + 32;
  sec_heap &= ~7;
  size_t sec_heap_sz = 0xC000UL - (sec_heap - (uintptr_t)XCHAL_INSTRAM1_VADDR);
  sec_heap_sz -= IRAM_RESERVE_SZ;  // Shrink IRAM heap
  if (0xC000UL > sec_heap_sz)
  {
    umm_init_iram_ex((void*)sec_heap, sec_heap_sz, true);
  }
}

#else
void setup()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(10);
  Serial.println("\r\n\r\nThis sketch requires Tools Option: 'MMU: 16KB cache + 48KB IRAM and 2nd Heap (shared)'");
}

void loop(void)
{
}
#endif
