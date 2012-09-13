#ifndef WL_OS_H
#define WL_OS_H

#include <stdarg.h>
#include <stdlib.h>

void *owl_os_alloc(size_t size);
void *owl_os_realloc(void *ptr, size_t size);
void owl_os_free(void *p);
void *owl_os_memcpy(void *dst, const void *src, size_t n);
void *owl_os_memset(void *s, int c, size_t n);
void *owl_os_memmove(void *dst, const void *src, size_t n);
size_t owl_os_strlen(char *s);
char *owl_os_strncpy(char *dst, const char *src, size_t n);
int owl_os_strncmp(const char *s1, const char *s2, size_t n);
int owl_os_strcmp(const char *s1, const char *s2);
char *owl_os_strcpy(char *dst, const char *src);
char *owl_os_strdup(const char *s);
char *owl_os_strndup(const char *s, size_t n);
int owl_os_memcmp(const void *s1, const void *s2, size_t n);
long int owl_os_strtol(const char *nptr, char **endptr, int base);
char *owl_os_strchr(const char *s, int c);
char *owl_os_strrchr(const char *s, int c);
int owl_os_strcasecmp(const char *s1, const char *s2);
char *owl_os_strstr(const char *haystack, const char *needle);

int owl_os_snprintf(char *str, size_t size, const char *format, ...)
        __attribute__((format(printf, 3, 4)));

int owl_os_vprintf(const char *format, va_list arg); /* debug only */
int owl_os_printf(const char *format, ...) /* debug only */
        __attribute__((format(printf, 1, 2)));

#endif /* WL_OS_H */

