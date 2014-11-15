#ifndef STDLIB_NONISO_H
#define STDLIB_NONISO_H

#ifdef __cplusplus
extern "C"{
#endif


char* itoa (int val, char *s, int radix);

char* ltoa (long val, char *s, int radix);

char* utoa (unsigned int val, char *s, int radix);

char* ultoa (unsigned long val, char *s, int radix);

char* dtostre (double val, char *s, unsigned char prec, unsigned char flags);
 
char* dtostrf (double val, signed char width, unsigned char prec, char *s);

#ifdef __cplusplus
} // extern "C"
#endif


#endif
