#ifndef __MIMETABLE_H__
#define __MIMETABLE_H__

#include "WString.h"

namespace mime
{

enum type
{
  html,
  htm,
  txt,
#ifndef MIMETYPE_MINIMAL    // allow to compile with only the strict minimum of mime-types
  css,
  js,
  json,
  png,
  gif,
  jpg,
  jpeg,
  ico,
  svg,
  ttf,
  otf,
  woff,
  woff2,
  eot,
  sfnt,
  xml,
  pdf,
  zip,
  appcache,
#endif // MIMETYPE_MINIMAL
  gz,
  none,
  maxType
};


extern const char * mimeTableSuffix[maxType];
extern const char * mimeTable[maxType];

String getContentType(const String& path);
}

#endif
