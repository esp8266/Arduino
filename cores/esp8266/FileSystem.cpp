/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/anakod/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 ****/

#include "FileSystem.h"
#include "WString.h"

file_t fileOpen(const String name, FileOpenFlags flags)
{
  int repeats = 0;
  bool notExist;
  bool canRecreate = (flags & eFO_CreateIfNotExist) == eFO_CreateIfNotExist;
  int res;

  do
  {
	  notExist = false;
	  res = SPIFFS_open(&_filesystemStorageHandle, name.c_str(), (spiffs_flags)flags, 0);
	  int code = SPIFFS_errno(&_filesystemStorageHandle);
	  if (res < 0)
	  {
		  debugf("open errno %d\n", code);
		  notExist = (code == SPIFFS_ERR_NOT_FOUND || code == SPIFFS_ERR_DELETED || code == SPIFFS_ERR_FILE_DELETED || code == SPIFFS_ERR_IS_FREE);
		  //debugf("recreate? %d %d %d", notExist, canRecreate, (repeats < 3));
		  if (notExist && canRecreate)
			  fileDelete(name); // fix for deleted files
	  }
  } while (notExist && canRecreate && repeats++ < 3);

  return res;
}

void fileClose(file_t file)
{
  SPIFFS_close(&_filesystemStorageHandle, file);
}

size_t fileWrite(file_t file, const void* data, size_t size)
{
  int res = SPIFFS_write(&_filesystemStorageHandle, file, (void *)data, size);
  if (res < 0)
  {
    debugf("write errno %d\n", SPIFFS_errno(&_filesystemStorageHandle));
    return res;
  }
  return res;
}

size_t fileRead(file_t file, void* data, size_t size)
{
  int res = SPIFFS_read(&_filesystemStorageHandle, file, data, size);
  if (res < 0)
  {
    debugf("read errno %d\n", SPIFFS_errno(&_filesystemStorageHandle));
    return res;
  }
  return res;
}

int fileSeek(file_t file, int offset, SeekOriginFlags origin)
{
  return SPIFFS_lseek(&_filesystemStorageHandle, file, offset, origin);
}

bool fileIsEOF(file_t file)
{
  return SPIFFS_eof(&_filesystemStorageHandle, file);
}

int32_t fileTell(file_t file)
{
  return SPIFFS_tell(&_filesystemStorageHandle, file);
}

int fileFlush(file_t file)
{
  return SPIFFS_fflush(&_filesystemStorageHandle, file);
}

int fileStats(const String name, spiffs_stat *stat)
{
	return SPIFFS_stat(&_filesystemStorageHandle, name.c_str(), stat);
}

int fileStats(file_t file, spiffs_stat *stat)
{
	return SPIFFS_fstat(&_filesystemStorageHandle, file, stat);
}

void fileDelete(const String name)
{
	SPIFFS_remove(&_filesystemStorageHandle, name.c_str());
}

void fileDelete(file_t file)
{
	SPIFFS_fremove(&_filesystemStorageHandle, file);
}

bool fileExist(const String name)
{
  spiffs_stat stat = {0};
  if (fileStats(name.c_str(), &stat) < 0) return false;
  return stat.name[0] != '\0';
}


int fileLastError(file_t fd)
{
  return SPIFFS_errno(&_filesystemStorageHandle);
}

void fileClearLastError(file_t fd)
{
  _filesystemStorageHandle.errno = SPIFFS_OK;
}

void fileSetContent(const String fileName, const char *content)
{
	file_t file = fileOpen(fileName.c_str(), eFO_CreateNewAlways | eFO_WriteOnly);
	fileWrite(file, content, os_strlen(content));
	fileClose(file);
}

uint32_t fileGetSize(const String fileName)
{
	file_t file = fileOpen(fileName.c_str(), eFO_ReadOnly);
	// Get size
	fileSeek(file, 0, eSO_FileEnd);
	int size = fileTell(file);
	fileClose(file);
	return size;
}

String fileGetContent(const String fileName)
{
	file_t file = fileOpen(fileName.c_str(), eFO_ReadOnly);
	// Get size
	fileSeek(file, 0, eSO_FileEnd);
	int size = fileTell(file);
	if (size <= 0)
	{
		fileClose(file);
		return "";
	}
	fileSeek(file, 0, eSO_FileStart);
	char* buffer = new char[size + 1];
	buffer[size] = 0;
	fileRead(file, buffer, size);
	fileClose(file);
	String res = buffer;
	delete[] buffer;
	return res;
}

int fileGetContent(const String fileName, char* buffer, int bufSize)
{
	if (buffer == NULL || bufSize == 0) return 0;
	*buffer = 0;

	file_t file = fileOpen(fileName.c_str(), eFO_ReadOnly);
	// Get size
	fileSeek(file, 0, eSO_FileEnd);
	int size = fileTell(file);
	if (size <= 0 || bufSize <= size)
	{
		fileClose(file);
		return 0;
	}
	buffer[size] = 0;
	fileSeek(file, 0, eSO_FileStart);
	fileRead(file, buffer, size);
	fileClose(file);
	return size;
}
