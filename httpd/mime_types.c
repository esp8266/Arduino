/*
 *  Copyright(C) 2006 Cameron Rich
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "os_port.h"

static const char mime_default[] = "text/plain";

typedef struct 
{
    const char * const ext;
    const char * const type;
} mime_table_t;

static mime_table_t mime_table[] = 
{
    // Fundamentals
    { ".html", "text/html" },
    { ".htm", "text/html" },
    { ".css", "text/css" },

    // Basic graphics
    { ".jpg", "image/jpeg" },
    { ".gif", "image/gif" },
    { ".png", "image/png" },

#ifdef CONFIG_HTTP_ALL_MIME_TYPES
    // This list is a bit expensive to maintain normally, so it's an option.
    { ".txt", "text/plain" },
    { ".rtx", "text/richtext" },
    { ".etx", "text/x-setext" },
    { ".tsv", "text/tab-separated-values" },
    { ".xml", "text/xml" },
    { ".dtd", "text/xml" },
    { ".jpe", "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".jfif", "image/jpeg" },
    { ".tif", "image/tiff" },
    { ".tiff", "image/tiff" },
    { ".pbm", "image/x-portable-bitmap" },
    { ".pgm", "image/x-portable-graymap" },
    { ".ppm", "image/x-portable-pixmap" },
    { ".pnm", "image/x-portable-anymap" },
    { ".xbm", "image/x-xbitmap" },
    { ".xpm", "image/x-xpixmap" },
    { ".xwd", "image/x-xwindowdump" },
    { ".ief", "image/ief" },
    { ".au", "audio/basic" },
    { ".snd", "audio/basic" },
    { ".aif", "audio/x-aiff" },
    { ".aiff", "audio/x-aiff" },
    { ".aifc", "audio/x-aiff" },
    { ".ra", "audio/x-pn-realaudio" },
    { ".ram", "audio/x-pn-realaudio" },
    { ".rm", "audio/x-pn-realaudio" },
    { ".rpm", "audio/x-pn-realaudio-plugin" },
    { ".wav", "audio/wav" },
    { ".mid", "audio/midi" },
    { ".midi", "audio/midi" },
    { ".kar", "audio/midi" },
    { ".mpga", "audio/mpeg" },
    { ".mp2", "audio/mpeg" },
    { ".mp3", "audio/mpeg" },
    { ".mpeg", "video/mpeg" },
    { ".mpg", "video/mpeg" },
    { ".mpe", "video/mpeg" },
    { ".qt", "video/quicktime" },
    { ".mov", "video/quicktime" },
    { ".avi", "video/x-msvideo" },
    { ".movie", "video/x-sgi-movie" },
    { ".mv", "video/x-sgi-movie" },
    { ".vx", "video/x-rad-screenplay" },
    { ".a", "application/octet-stream" },
    { ".bin", "application/octet-stream" },
    { ".exe", "application/octet-stream" },
    { ".dump", "application/octet-stream" },
    { ".o", "application/octet-stream" },
    { ".class", "application/java" },
    { ".js", "application/x-javascript" },
    { ".ai", "application/postscript" },
    { ".eps", "application/postscript" },
    { ".ps", "application/postscript" },
    { ".dir", "application/x-director" },
    { ".dcr", "application/x-director" },
    { ".dxr", "application/x-director" },
    { ".fgd", "application/x-director" },
    { ".aam", "application/x-authorware-map" },
    { ".aas", "application/x-authorware-seg" },
    { ".aab", "application/x-authorware-bin" },
    { ".fh4", "image/x-freehand" },
    { ".fh7", "image/x-freehand" },
    { ".fh5", "image/x-freehand" },
    { ".fhc", "image/x-freehand" },
    { ".fh", "image/x-freehand" },
    { ".spl", "application/futuresplash" },
    { ".swf", "application/x-shockwave-flash" },
    { ".dvi", "application/x-dvi" },
    { ".gtar", "application/x-gtar" },
    { ".hdf", "application/x-hdf" },
    { ".hqx", "application/mac-binhex40" },
    { ".iv", "application/x-inventor" },
    { ".latex", "application/x-latex" },
    { ".man", "application/x-troff-man" },
    { ".me", "application/x-troff-me" },
    { ".mif", "application/x-mif" },
    { ".ms", "application/x-troff-ms" },
    { ".oda", "application/oda" },
    { ".pdf", "application/pdf" },
    { ".rtf", "application/rtf" },
    { ".bcpio", "application/x-bcpio" },
    { ".cpio", "application/x-cpio" },
    { ".sv4cpio", "application/x-sv4cpio" },
    { ".sv4crc", "application/x-sv4crc" },
    { ".sh", "application/x-shar" },
    { ".shar", "application/x-shar" },
    { ".sit", "application/x-stuffit" },
    { ".tar", "application/x-tar" },
    { ".tex", "application/x-tex" },
    { ".texi", "application/x-texinfo" },
    { ".texinfo", "application/x-texinfo" },
    { ".tr", "application/x-troff" },
    { ".roff", "application/x-troff" },
    { ".man", "application/x-troff-man" },
    { ".me", "application/x-troff-me" },
    { ".ms", "application/x-troff-ms" },
    { ".zip", "application/x-zip-compressed" },
    { ".tsp", "application/dsptype" },
    { ".wsrc", "application/x-wais-source" },
    { ".ustar", "application/x-ustar" },
    { ".cdf", "application/x-netcdf" },
    { ".nc", "application/x-netcdf" },
    { ".doc", "application/msword" },
    { ".ppt", "application/powerpoint" },
    { ".wrl", "model/vrml" },
    { ".vrml", "model/vrml" },
    { ".mime", "message/rfc822" },
    { ".pac", "application/x-ns-proxy-autoconfig" },
    { ".wml", "text/vnd.wap.wml" },
    { ".wmlc", "application/vnd.wap.wmlc" },
    { ".wmls", "text/vnd.wap.wmlscript" },
    { ".wmlsc", "application/vnd.wap.wmlscriptc" },
    { ".wbmp", "image/vnd.wap.wbmp" },
    { ".tgz", "application/x-gzip" },
    { ".tar.gz", "application/x-gzip" },
    { ".bz2", "application/x-bzip2" },
    { ".zip", "application/zip" }
#endif
};

static int mime_cmp(const mime_table_t *t1, const mime_table_t *t2)
{
    return strcasecmp(t1->ext, t2->ext);
}

void mime_init(void)
{
    qsort(mime_table, sizeof(mime_table)/sizeof(mime_table_t),
            sizeof(mime_table_t), 
            (int (*)(const void *, const void *))mime_cmp);
}

const char *getmimetype(const char *name) 
{
    mime_table_t *mime_type;

    if ((name = strrchr(name, '.')) == NULL)
        return mime_default;

    mime_type = bsearch(&name, mime_table, 
            sizeof(mime_table)/sizeof(mime_table_t),
            sizeof(mime_table_t), 
                (int (*)(const void *, const void *))mime_cmp);

    return mime_type == NULL ? mime_default : mime_type->type;
}

