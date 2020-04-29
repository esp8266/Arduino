# FSBrowser readme

## What is this sketch about ?

This example is a FileSystem Browser for the ESP8266 using http requests and a html/javascript frontend, 
working for all of SPIFFS, LittleFS and SDFS.
This unified version is based on the previous examples named FSWebServer, FSBrowser and SDWebServer, Copyright (c) 2015 Hristo Gochkov. All rights reserved.

## How to use it ?
1. Uncomment one of the `#define USE_xxx` directives in the sketch
2. Add the credentials of your WiFi network (search for `STASSID`)
3. Compile and upload the sketch to your ESP8266 device
4. For normal use, copy the contents of the `data` folder to the filesystem. To do so:
- for SDFS, copy that contents (not the data folder itself, just its contents) to the root of a FAT/FAT32-formated SD card connected to the SPI port of the ESP8266
- for SPIFFS or LittleFS, please follow the instructions at https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system
5. Once the data and sketch have been uploaded, access the editor by pointing your browser to http://fsbrowser.local/edit

## Options
If you need to free some space on the ESP filesystem, you can delete all the sample files at the root but also replace the `index.htm` file in the `data/edit` subfolder by the `index.htm.gz` file from the `extras` folder. That compressed version is not suited for learning or debugging, but will bring the total FS usage under 7KB.
If you want to use the browser on a an existing filesystem or don't want to perform step 4 above, you have two possibilities :
- either upload the `index.htm` file to the filesystem by opening a command shell in the `data` folder and running the following cURL command:
`curl -F file=@edit/index.htm;filename=/edit/index.htm fsbrowser.local/edit`
- or embed a version of the html page in the source code itself by uncommenting the following line in the sketch and rebuilding:
`#define INCLUDE_FALLBACK_INDEX_HTM`
That embedded version is functionally equivalent and will be returned if no `/edit/index.htm` or `/edit/index.htm.gz` file can be found on the filesystem, at the expense of a higher binary size.

If you use the gzipped or `INCLUDE_FALLBACK_INDEX_HTM` options, please remember to rerun the `reduce_index.sh` script located in the `extras` subfolder and recompile the sketch after each change to the `index.html` file.

## Dependency
The html page uses the [Ace.js](https://ace.c9.io/) (v1.4.9 at the time of writing) text editor which is loaded from a CDN. Consequently, internet access from your web browser is required for the FSBrowser editing feature to work as-is.

If your browser has no web access (e.g. if you are connected to the ESP8266 as an access-point), you can copy the `ace.js` file to the `edit` subfolder of the ESP filesystem, along with optional plugins etc. according to your needs. A typical set might be:
```
ace.js
ext-keybinding_menu.js
ext-searchbox.js
mode-html.js
worker-html.js
worker-css.js
worker-javascript.js
mode-xml.js
worker-xml.js
mode-json.js
worker-json.js
```
(see https://github.com/ajaxorg/ace-builds for a full list).

If `ace.js` cannot be found on the ESP filesystem either, the page will default to a plain text viewer, with a warning message.

## Notes
- See https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html for more information on FileSystems supported by the ESP8266.
- For SDFS, if your card's CS pin is not connected to the default pin (4), uncomment the `fileSystemConfig.setCSPin(chipSelectPin);` line, specifying the GPIO the CS pin is connected to
- `index.htm` is the default index returned if your URL does not end with a filename (works on subfolders as well)
- Filesystem limitations apply. For example, FAT16 is limited to 8.3 filenames - see https://en.wikipedia.org/wiki/8.3_filename - SPIFFS and LittleFS also have limitations, please see https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#spiffs-file-system-limitations
- Directories are supported on SDFS and LittleFS. On SPIFFS, all files are at the root, although their names may contain the "/" character.
- The convention here is that the root of the filesystem is "/". On SPIFFS, paths not started with a slash are not supported
- For creation, the convention is that a path ending with a "/" means create a folder, while without a "/" we create a file. Having an extension or not does not matter.

## Changelog since original FSBrowser
 
### Fixes to work on LittleFS based on SDFS
- #define logic to select FS
- switched from SD to SDFS
- begin() does not support parameters > removed SS and added optional config
- LittleFS.open() second parametsr is mandatory > specified "r" where needed
- 'FILE_WRITE' was not declared in this scope > replaced by "w"

### UI/usability improvements
- Never format filesystem, just return "FS INIT ERROR" when FS cannot be mounted 
- Tree panel width is now proportional (20%) to see long names on big screens 
- Added icons for files, and indented them to the same level as folders
- Changed file/folder icon set to use a lighter and more neutral one, and added specific "text" and "image" icons for formats recognized as such
- Items are now sorted (folders first, then plain files, each in alphabetic order)
- Added file size after each file name
- Added FS status information at the top right
- Made clear that an async operation is in progress by dimming screen and showing operation status
- Filled filename box in header with the name of the last clicked file
- Selecting a file for upload defaults to putting it in the same folder as the last clicked file
- Removed limitation to 8.3 lowercase filenames
- Support Filenames without extension, Dirnames with extension
- Improved recursive refresh of parts of the tree (e.g. refresh folder upon file delete, show last folder upon creating nested file)
- Added Save/Discard/Help buttons to ACE editor, discard confirmation on leave, and refresh tree and status upon save
- Removed "Upload" from context menu (which didn't work anyway)
- Added "Rename/Move" feature to context menu
- Sketch can be used on a preexisting filesystem by embedding the index.htm file in the program

## TODO (maybe)
- ? How can we query the fatType of the SDFS (FAT16 or FAT32) to limit filenames to 8.3 on FAT16 ?
- ? Add a visible root node "/" (with no delete option) + add the FS type next to it, like <i>LittleFS</i> 
- ? move "Mkdir" and "MkFile" to context menu, with prompt like for Rename/Move
- ? implement drag/drop for move + make "rename" only a local rename operation (no move)
- ? Optionally present SPIFFS as a hierarchical FS too
- ? Optionally mount several filesystems at the same time (SPIFFS + SDFS or LittleFS + SDFS)

## Test suite
These tests are a checklist of operations to verify the FSBrowser behaviour.
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
- Original version of FSBrowser written by me-no-dev, contributions over time by various contributors
- Icons are from https://feathericons.com/ . The resulting PNG is passed first through https://compresspng.com/ before being converted to base64 using https://www.base64-image.de/
- The spinner is based on https://github.com/jlong/css-spinners
- Minifiying of index.htm is done using the command line version of https://kangax.github.io/html-minifier/
- Idea of embedding webpage in code borrowed from https://github.com/me-no-dev/ESPAsyncWebServer

