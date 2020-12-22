#include "mimetable.h"
#include "pgmspace.h"
#include "WString.h"

namespace mime
{

static const char kHtmlSuffix[] PROGMEM = ".html";
static const char kHtmSuffix[] PROGMEM = ".htm";
static const char kTxtSuffix[] PROGMEM = ".txt";
#ifndef MIMETYPE_MINIMAL
static const char kCssSuffix[] PROGMEM = ".css";
static const char kJsSuffix[] PROGMEM = ".js";
static const char kJsonSuffix[] PROGMEM = ".json";
static const char kPngSuffix[] PROGMEM = ".png";
static const char kGifSuffix[] PROGMEM = ".gif";
static const char kJpgSuffix[] PROGMEM = ".jpg";
static const char kJpegSuffix[] PROGMEM = ".jpeg";
static const char kIcoSuffix[] PROGMEM = ".ico";
static const char kSvgSuffix[] PROGMEM = ".svg";
static const char kTtfSuffix[] PROGMEM = ".ttf";
static const char kOtfSuffix[] PROGMEM = ".otf";
static const char kWoffSuffix[] PROGMEM = ".woff";
static const char kWoff2Suffix[] PROGMEM = ".woff2";
static const char kEotSuffix[] PROGMEM = ".eot";
static const char kSfntSuffix[] PROGMEM = ".sfnt";
static const char kXmlSuffix[] PROGMEM = ".xml";
static const char kPdfSuffix[] PROGMEM = ".pdf";
static const char kZipSuffix[] PROGMEM = ".zip";
static const char kAppcacheSuffix[] PROGMEM = ".appcache";
#endif // MIMETYPE_MINIMAL
static const char kGzSuffix[] PROGMEM = ".gz";
static const char kDefaultSuffix[] PROGMEM = "";

static const char kHtml[] PROGMEM = "text/html";
static const char kTxt[] PROGMEM = "text/plain";
#ifndef MIMETYPE_MINIMAL
static const char kCss[] PROGMEM = "text/css";
static const char kJs[] PROGMEM = "application/javascript";
static const char kJson[] PROGMEM = "application/json";
static const char kPng[] PROGMEM = "image/png";
static const char kGif[] PROGMEM = "image/gif";
static const char kJpg[] PROGMEM = "image/jpeg";
static const char kJpeg[] PROGMEM = "image/jpeg";
static const char kIco[] PROGMEM = "image/x-icon";
static const char kSvg[] PROGMEM = "image/svg+xml";
static const char kTtf[] PROGMEM = "application/x-font-ttf";
static const char kOtf[] PROGMEM = "application/x-font-opentype";
static const char kWoff[] PROGMEM = "application/font-woff";
static const char kWoff2[] PROGMEM = "application/font-woff2";
static const char kEot[] PROGMEM = "application/vnd.ms-fontobject";
static const char kSfnt[] PROGMEM = "application/font-sfnt";
static const char kXml[] PROGMEM = "text/xml";
static const char kPdf[] PROGMEM = "application/pdf";
static const char kZip[] PROGMEM = "application/zip";
static const char kAppcache[] PROGMEM = "text/cache-manifest";
#endif // MIMETYPE_MINIMAL
static const char kGz[] PROGMEM = "application/x-gzip";
static const char kDefault[] PROGMEM = "application/octet-stream";

const Entry mimeTable[maxType] PROGMEM =
{
    { kHtmlSuffix, kHtml },
    { kHtmSuffix, kHtml },
    { kTxtSuffix, kTxt },
#ifndef MIMETYPE_MINIMAL
    { kCssSuffix, kCss },
    { kJsSuffix, kJs },
    { kJsonSuffix, kJson },
    { kPngSuffix, kPng },
    { kGifSuffix, kGif },
    { kJpgSuffix, kJpg },
    { kJpegSuffix, kJpeg },
    { kIcoSuffix, kIco },
    { kSvgSuffix, kSvg },
    { kTtfSuffix, kTtf },
    { kOtfSuffix, kOtf },
    { kWoffSuffix, kWoff },
    { kWoff2Suffix, kWoff2 },
    { kEotSuffix, kEot },
    { kSfntSuffix, kSfnt },
    { kXmlSuffix, kXml },
    { kPdfSuffix, kPdf },
    { kZipSuffix, kZip },
    { kAppcacheSuffix, kAppcache },
#endif // MIMETYPE_MINIMAL
    { kGzSuffix, kGz },
    { kDefaultSuffix, kDefault }
};

    String getContentType(const String& path) {
        for (size_t i = 0; i < maxType; i++) {
            if (path.endsWith(FPSTR(mimeTable[i].endsWith))) {
                return String(FPSTR(mimeTable[i].mimeType));
            }
        }
        // Fall-through and just return default type
        return String(FPSTR(kDefault));
    }

}
