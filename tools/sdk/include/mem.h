#ifndef __MEM_H__
#define __MEM_H__

//void *pvPortMalloc( size_t xWantedSize );
//void vPortFree( void *pv );
//void *pvPortZalloc(size_t size);

#define os_malloc   pvPortMalloc
#define os_free     vPortFree
#define os_zalloc   pvPortZalloc
#define os_realloc  pvPortRealloc

#endif
