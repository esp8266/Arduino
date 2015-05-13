/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/anakod/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 ****/

#ifndef _SMING_CORE_FILESYSTEM_H_
#define _SMING_CORE_FILESYSTEM_H_

#include "spiffs/spiffs.h"
class String;

enum FileOpenFlags
{
  eFO_ReadOnly = SPIFFS_RDONLY,
  eFO_WriteOnly = SPIFFS_WRONLY,
  eFO_ReadWrite = eFO_ReadOnly | eFO_WriteOnly,
  eFO_CreateIfNotExist = SPIFFS_CREAT,
  eFO_Append = SPIFFS_APPEND,
  eFO_Truncate = SPIFFS_TRUNC,
  eFO_CreateNewAlways = eFO_CreateIfNotExist | eFO_Truncate
};

static FileOpenFlags operator|(FileOpenFlags lhs, FileOpenFlags rhs)
{
    return (FileOpenFlags) ((int)lhs| (int)rhs);
}

typedef enum
{
	eSO_FileStart = SPIFFS_SEEK_SET,
	eSO_CurrentPos = SPIFFS_SEEK_CUR,
	eSO_FileEnd = SPIFFS_SEEK_END
} SeekOriginFlags;

file_t fileOpen(const String name, FileOpenFlags flags);
void fileClose(file_t file);
size_t fileWrite(file_t file, const void* data, size_t size);
size_t fileRead(file_t file, void* data, size_t size);
int fileSeek(file_t file, int offset, SeekOriginFlags origin);
bool fileIsEOF(file_t file);
int32_t fileTell(file_t file);
int fileFlush(file_t file);
int fileLastError(file_t fd);
void fileClearLastError(file_t fd);
void fileSetContent(const String fileName, const char *content);
uint32_t fileGetSize(const String fileName);
String fileGetContent(const String fileName);
int fileGetContent(const String fileName, char* buffer, int bufSize);


int fileStats(const String name, spiffs_stat *stat);
int fileStats(file_t file, spiffs_stat *stat);
void fileDelete(const String name);
void fileDelete(file_t file);
bool fileExist(const String name);

#endif /* _SMING_CORE_FILESYSTEM_H_ */
