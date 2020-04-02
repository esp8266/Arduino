# FSBrowser readme

## What is this sketch about ?

This example is a FileSystem browser for the ESP8266 using http requests and a html/javascript frontend, 
working for both SPIFFS, LittleFS and SDFS.
This unified version is based on the previous examples named FSWebServer, FSBrowser and SDWebServer, Copyright (c) 2015 Hristo Gochkov. All rights reserved.

## How to use ?

1. Uncomment one of the "#define USE_xxx" directives in the sketch
2. Add the credentials of your WiFi network (search for "STASSID")
3. Copy the contents of the 'data' folder to the filesystem. To do so:
- for SDFS, copy that contents to the root of a FAT/FAT32-formated SD card connected to the SPI port of the ESP8266
- for SPIFFS or LittleFS, please follow the instructions at https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system
4. Once the data and sketch have been uploaded, access the editor by pointing your browser to http://fsbrowser.local/edit
5. If you need to free as much space as possible, you can delete the sample files at the root but also the /edit/index.htm file, because the compressed version /edit/index.htm.gz will be used instead, for a total usage of less than 7KB

## Notes
- See https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html for more information on FileSystems supported by the ESP8266.
- For SDFS, if your card's CS pin is not connected the default pin (4), enable the line "fileSystemConfig.setCSPin(chipSelectPin);" specifying the GPIO the CS pin is connected to
- index.htm is the default index (works on subfolders as well)
- Filesystem limitations apply. For example, FAT16 is limited to 8.3 filenames - https://en.wikipedia.org/wiki/8.3_filename - SPIFFS and LittleFS also have limitations, please see https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#spiffs-file-system-limitations
- Directories are supported on SDFS and LittleFS. On SPIFFS, all files are at the root, although their names may contain the "/" character
- The convention here is that the root of the filesystem is "/". On SPIFFS, paths not started with a slash are not supported
- For creation, the convention is that a path ending with a "/" means create a folder, while without a "/" we create a file. Having an extension or not does not matter.

## Changeslog since original FSBrowser
 
### Fixes to work on LittleFS based on SDFS
- #define logic to select FS
- switched from SD to SDFS
- begin() does not support parameters > removed SS and added optional config
- LittleFS.open() second parametsr is mandatory > specified "r" where needed
- 'FILE_WRITE' was not declared in this scope > replaced by "w"

### UI/usability improvements
- Never format filesystem, just return "FS INIT ERROR" when FS could not be mounted 
- Tree panel width is now proportional (20%) to see long names on big screens 
- Added an icon for files, and indented them to the same level as folders
- Changed file/folder icon set to use lighter and more neutral ones, and added specific "text" and "image" icons for formats recognized as such
- Items are now sorted (folders first, then plain files, each in alphabetic order)
- Added file size after each file name
- Added FS status information at the top right
- Made clear that an async operation is in progress by dimming screen and showing opertation status
- Filled filename box in header with the name of the last clicked file
- Selecting a file for upload defaults to putting it in the same folder as the last clicked file
- Removed limitation to 8.3 lowercase filenames
- Support Filenames without extension, Dirnames with extension
- Improved recursive refresh of parts of the tree (e.g. refresh folder upon file delete, show last folder upon creating nested file)
- Added Save/Discard/Help buttons to ACE editor, discard confirmation on leave, and refresh tree and status upon save
- Removed "Upload" from context menu (which didn't work anyway)
- Added "Rename/Move" feature to context menu

## TODO (maybe)
- ? Is there a case where we need to limit files to 8.3 ? FAT16 maybe ?
- ? If so, how can we query the fatType of the SDFS (FAT16 or FAT32)
- ? Add a visible root node "/" (with no delete option) + add the FS type next to it, like <i>LittleFS</i> 
- ? move "Mkdir" and "MkFile" to context menu, with prompt like for Rename/Move
- ? implement drag/drop for move + make "rename" only a local rename operation (no move)
- ? Optionally present SPIFFS as a hierarchical FS too
- ? Check if internet access is available and and replace ace editor by a text viewer otherwise
- ? Optionally mount several filesystems at the same time (SPIFFS + SDFS or LittleFS + SDFS)

## Test suite
 
### On SPIFFS
#### 8.3 filenames
- At root : MkFile '/1.txt' / List / Edit / Download / Delete / Upload '/1.png' / View image / Delete image
- In subdir : MkFile '/dir/2.txt' / List / Edit / Download / Delete / Upload '/dir/2.png' / View image
- Create nested file '/a/b.txt' and delete it
- Attempt creation of unsupported filenames
#### Long filenames
- At root : MkFile '/My text file 1.txt' / List / Edit / Download / Delete / Upload '/My image file 1.png' / View image / Delete image
- In subdir : MkFile '/My Directory/My text 2.txt' / List / Edit / Download / Delete / Upload '/My Directory/My image 2.png' / View image
- Create nested file '/My folder/My test file.txt' and delete it

### On LittleFS
#### 8.3 filenames
- At root : MkFile '/1.txt' / List / Edit / Download / Delete / Upload '/1.png' / View image / Delete image / Mkdir '/dir'
- In subdir : MkFile '/dir/2.txt' / List / Edit / Download / Delete / Upload '/dir/2.png' / View image / Mkdir '/dir/sub'
- Delete root folder '/dir'
- Create nested file '/a/b.txt' and delete file 'b.txt'
#### Long filenames
- At root : MkFile '/My text file 1.txt' / List / Edit / Download / Delete / Upload '/My image file 1.png' / View image / Delete image / Mkdir '/My Directory'
- In subdir : MkFile '/My Directory/My text file 2.txt' / List / Edit / Download / Delete / Upload '/My Directory/My image file 2.png' / View image / Mkdir '/My Directory/My Subdirectory'
- Delete root folder '/My Directory'
- Create nested file '/My folder/My test file.txt' and delete file 'My test file.txt' 
 
### On SDFS
#### 8.3 filenames
- At root : MkFile '/1.txt' / List / Edit / Download / Delete / Upload '/1.png' / View image / Delete image / Mkdir '/dir'
- In subdir : MkFile '/dir/2.txt' / List / Edit / Download / Delete / Upload '/dir/2.png' / View image / Mkdir '/dir/sub'
- Delete root folder '/dir'
- Create nested file '/a/b.txt' and delete file 'b.txt', then delete '/a'
#### Long filenames
- At root : MkFile '/My text file 1.txt' / List / Edit / Download / Delete / Upload '/My image file 1.png' / View image / Delete image / Mkdir '/My Directory'
- In subdir : MkFile '/My Directory/My text file 2.txt' / List / Edit / Download / Delete / Upload '/My Directory/My image file 2.png' / View image / Mkdir '/My Directory/My Subdirectory'
- Delete root folder '/My Directory'
- Create nested file '/My folder/My test file.txt' and delete file 'My test file.txt' 

## Credits
- Icons are from https://feathericons.com/ . The resulting PNG is passed first through https://compresspng.com/ before converting it using https://www.base64-image.de/
- Spinner is based on https://github.com/jlong/css-spinners
- Minifiying of index.htm done by : https://css-minifier.com/ (css) + https://www.javascriptminifier.com/ (js) + https://www.willpeavy.com/tools/minifier/ (html)

