#ifndef _ITOA_
#define _ITOA_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#if 0

extern void itoa( int n, char s[] ) ;

#else

extern char* itoa( int value, char *string, int radix ) ;
extern char* ltoa( long value, char *string, int radix ) ;
extern char* utoa( unsigned long value, char *string, int radix ) ;
extern char* ultoa( unsigned long value, char *string, int radix ) ;
#endif /* 0 */

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // _ITOA_
