#ifndef __MEM_MANAGER_H__
#define __MEM_MANAGER_H__

#include "c_types.h"

/*------------------------变量定义------------------------*/

#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE
#ifndef IOT_SIP_MODE
//#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 0x3fffc000 - (uint32)&_heap_start ) )//fix 16000 to 24000 on 14.2.26
#else
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 8000 ) )
#endif
#define portBYTE_ALIGNMENT			8
#define pdFALSE 0
#define pdTRUE  1

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE
#if portBYTE_ALIGNMENT == 8
	#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#endif

#if portBYTE_ALIGNMENT == 4
	#define portBYTE_ALIGNMENT_MASK	( 0x0003 )
#endif

#if portBYTE_ALIGNMENT == 2
	#define portBYTE_ALIGNMENT_MASK	( 0x0001 )
#endif

#if portBYTE_ALIGNMENT == 1
	#define portBYTE_ALIGNMENT_MASK	( 0x0000 )
#endif

#ifndef portBYTE_ALIGNMENT_MASK
	#error "Invalid portBYTE_ALIGNMENT definition"
#endif

#define configUSE_MALLOC_FAILED_HOOK	1
#define portPOINTER_SIZE_TYPE unsigned int

#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

//#define configADJUSTED_HEAP_SIZE	( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )

//static unsigned char ucHeap[ configTOTAL_HEAP_SIZE ];
static unsigned char *ucHeap;

typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	//The next free block in the list. 
	size_t xBlockSize;						//The size of the free block. 
} xBlockLink;

static const unsigned short heapSTRUCT_SIZE	= ( sizeof( xBlockLink ) + portBYTE_ALIGNMENT - ( sizeof( xBlockLink ) % portBYTE_ALIGNMENT ) );

//static const size_t xTotalHeapSize = ( ( size_t ) configADJUSTED_HEAP_SIZE ) & ( ( size_t ) ~portBYTE_ALIGNMENT_MASK );

static xBlockLink xStart, *pxEnd = NULL;

//static size_t xFreeBytesRemaining = ( ( size_t ) configADJUSTED_HEAP_SIZE ) & ( ( size_t ) ~portBYTE_ALIGNMENT_MASK );


/*------------------------函数声明-----------------------------------*/

static void prvInsertBlockIntoFreeList( xBlockLink *pxBlockToInsert ) ;//ICACHE_FLASH_ATTR;

static void prvHeapInit( void ) ;//ICACHE_FLASH_ATTR;

void vApplicationMallocFailedHook( void ) ;//ICACHE_FLASH_ATTR;

void *pvPortMalloc( size_t xWantedSize ) ;//ICACHE_FLASH_ATTR;

void vPortFree( void *pv ) ;//ICACHE_FLASH_ATTR;

size_t xPortGetFreeHeapSize( void ) ;//ICACHE_FLASH_ATTR;

void vPortInitialiseBlocks( void ) ;//ICACHE_FLASH_ATTR;
/*-----------------------------------------------------------*/

#endif
