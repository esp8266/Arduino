// https://gist.github.com/Fonger/98cc95ac39fbe1a7e4d9

#include <cstddef>
#include <cstdlib>
#include <cstring>

extern "C"
{
#ifdef STRLCAT_MISSING
    // '_cups_strlcat()' - Safely concatenate two strings.

    size_t                   /* O - Length of string */
    strlcat(char*       dst, /* O - Destination string */
            const char* src, /* I - Source string */
            size_t      size)     /* I - Size of destination string buffer */
    {
        size_t srclen; /* Length of source string */
        size_t dstlen; /* Length of destination string */

        // Figure out how much room is left...

        dstlen = strlen(dst);
        size -= dstlen + 1;

        if (!size)
        {
            return (dstlen); /* No room, return immediately... */
        }

        // Figure out how much room is needed...

        srclen = strlen(src);

        // Copy the appropriate amount...

        if (srclen > size)
        {
            srclen = size;
        }

        memcpy(dst + dstlen, src, srclen);
        dst[dstlen + srclen] = '\0';

        return (dstlen + srclen);
    }
#endif /* STRLCAT_MISSING */

#ifdef STRLCPY_MISSING
    // '_cups_strlcpy()' - Safely copy two strings.

    size_t                   /* O - Length of string */
    strlcpy(char*       dst, /* O - Destination string */
            const char* src, /* I - Source string */
            size_t      size)     /* I - Size of destination string buffer */
    {
        size_t srclen; /* Length of source string */

        // Figure out how much room is needed...

        size--;

        srclen = strlen(src);

        // Copy the appropriate amount...

        if (srclen > size)
        {
            srclen = size;
        }

        memcpy(dst, src, srclen);
        dst[srclen] = '\0';

        return (srclen);
    }
#endif /* STRLCPY_MISSING */

}  // extern "C"
