#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <wl_os.h>

void *owl_os_alloc(size_t size)
{
        return malloc(size);
}

void *owl_os_realloc(void *ptr, size_t size)
{
        return realloc(ptr, size);
}
     
void owl_os_free(void *p)
{
        free(p);
}

void *owl_os_memcpy(void *dst, const void *src, size_t n)
{
	return memcpy(dst, src, n);
}

void *owl_os_memset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}

void *owl_os_memmove(void *dst, const void *src, size_t n)
{
	return memmove(dst, src, n);
}

size_t owl_os_strlen(char *s)
{
	return strlen(s);
}

char *owl_os_strncpy(char *dst, const char *src, size_t n)
{
	return strncpy(dst, src, n);
}

int owl_os_strncmp(const char *s1, const char *s2, size_t n)
{
	return strncmp(s1, s2, n);
}

int owl_os_strcmp(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}

char *owl_os_strcpy(char *dst, const char *src)
{
	return strcpy(dst, src);
}

char *owl_os_strdup(const char *s)
{
	return strdup(s);
}

char *owl_os_strndup(const char *s, size_t n)
{
	return strndup(s, n);
}

int owl_os_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n);
}

long int owl_os_strtol(const char *nptr, char **endptr, int base)
{
	return strtol(nptr, endptr, base);
}

char *owl_os_strchr(const char *s, int c)
{
	return strchr(s, c);
}

char *owl_os_strrchr(const char *s, int c)
{
	return strrchr(s, c);
}

int owl_os_strcasecmp(const char *s1, const char *s2)
{
	return strcasecmp(s1, s2);
}

char *owl_os_strstr(const char *haystack, const char *needle)
{
	return strstr(haystack, needle);
}

int owl_os_snprintf(char *str, size_t size, const char *format, ...)
{
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsniprintf(str, size, format, ap);
	va_end(ap);
	return ret;
}

/* for debugging only, never called if wl_api was built without debug */
#ifdef CONFIG_OWL
#include "owl_env.h"
int owl_os_printf(const char *fmt, ...)
{
        char *str = NULL;
        va_list args;
        int len;
        char *iter;

        va_start(args, fmt);

        if ((str = malloc(160)) == NULL)
                return -1;

        if ((len = vsniprintf(str, 160, fmt, args)) < 0) {
                free(str);
                return -1;
        }

        iter = str;
        while (*iter)
                owl_putc(*iter++);

        free(str);
        return len;
}
#endif
