Filesystem
==========


Flash layout
------------

Even though file system is stored on the same flash chip as the program,
programming new sketch will not modify file system contents. This allows
to use file system to store sketch data, configuration files, or content
for Web server.

The following diagram illustrates flash layout used in Arduino
environment:

::

    |--------------|-------|---------------|--|--|--|--|--|
    ^              ^       ^               ^     ^
    Sketch    OTA update   File system   EEPROM  WiFi config (SDK)

File system size depends on the flash chip size. Depending on the board
which is selected in IDE, you have the following options for flash size:

+---------------------------------+--------------------------+---------------------------+
| Board                           | Flash chip size, bytes   | File system size, bytes   |
+=================================+==========================+===========================+
| Generic module                  | 512k                     | 64k, 128k                 |
+---------------------------------+--------------------------+---------------------------+
| Generic module                  | 1M                       | 64k, 128k, 256k, 512k     |
+---------------------------------+--------------------------+---------------------------+
| Generic module                  | 2M                       | 1M                        |
+---------------------------------+--------------------------+---------------------------+
| Generic module                  | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| Adafruit HUZZAH                 | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| ESPresso Lite 1.0               | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| ESPresso Lite 2.0               | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| NodeMCU 0.9                     | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| NodeMCU 1.0                     | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| Olimex MOD-WIFI-ESP8266(-DEV)   | 2M                       | 1M                        |
+---------------------------------+--------------------------+---------------------------+
| SparkFun Thing                  | 512k                     | 64k                       |
+---------------------------------+--------------------------+---------------------------+
| SweetPea ESP-210                | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| WeMos D1 R1, R2 & mini          | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| ESPDuino                        | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+
| WiFiduino                       | 4M                       | 1M, 2M, 3M                |
+---------------------------------+--------------------------+---------------------------+

**Note:** to use any of file system functions in the sketch, add the
following include to the sketch:

.. code:: cpp

    #include "FS.h"

SPIFFS and LittleFS
-------------------

There are two filesystems for utilizing the onboard flash on the ESP8266:
SPIFFS and LittleFS.

SPIFFS is the original filesystem and is ideal for space and RAM
constrained applications that utilize many small files and care
about static and dynamic wear levelling and don't need true directory
support.  Filesystem overhead on the flash is minimal as well.

LittleFS is recently added and focuses on higher performance and
directory support, but has higher filesystem and per-file overhead
(4K minimum vs. SPIFFS' 256 byte minimum file allocation unit).

They share a compatible API but have incompatible on-flash
implementations, so it is important to choose one or the per project
as attempting to mount a SPIFFS volume under LittleFS may result
in a format operation and definitely will not preserve any files,
and vice-versa.

The actual ``File`` and ``Dir`` objects returned from either
filesystem behave in the same manner and documentation is applicable
to both.  To convert most applications from SPIFFS to LittleFS
simply requires changing the ``SPIFFS.begin()`` to ``LittleFS.begin()``
and ``SPIFFS.open()`` to ``LittleFS.open()`` with the rest of the
code remaining untouched.


SPIFFS file system limitations
------------------------------

The SPIFFS implementation for ESP8266 had to accomodate the
constraints of the chip, among which its limited RAM.
`SPIFFS <https://github.com/pellepl/spiffs>`__ was selected because it
is designed for small systems, but that comes at the cost of some
simplifications and limitations.

First, behind the scenes, SPIFFS does not support directories, it just
stores a "flat" list of files. But contrary to traditional filesystems,
the slash character ``'/'`` is allowed in filenames, so the functions
that deal with directory listing (e.g. ``openDir("/website")``)
basically just filter the filenames and keep the ones that start with
the requested prefix (``/website/``). Practically speaking, that makes
little difference though.

Second, there is a limit of 32 chars in total for filenames. One
``'\0'`` char is reserved for C string termination, so that leaves us
with 31 usable characters.

Combined, that means it is advised to keep filenames short and not use
deeply nested directories, as the full path of each file (including
directories, ``'/'`` characters, base name, dot and extension) has to be
31 chars at a maximum. For example, the filename
``/website/images/bird_thumbnail.jpg`` is 34 chars and will cause some
problems if used, for example in ``exists()`` or in case another file
starts with the same first 31 characters.

**Warning**: That limit is easily reached and if ignored, problems might
go unnoticed because no error message will appear at compilation nor
runtime.

For more details on the internals of SPIFFS implementation, see the
`SPIFFS readme
file <https://github.com/esp8266/Arduino/blob/master/cores/esp8266/spiffs/README.md>`__.


LittleFS file system limitations
--------------------------------

The LittleFS implementation for the ESP8266 supports filenames of up
to 31 characters + terminating zero (i.e. ``char filename[32]``), and
as many subdirectories as space permits.

Filenames are assumed to be in the root directory if no initial "/" is
present.

Opening files in subdirectories requires specifying the complete path to
the file (i.e. ``open("/sub/dir/file.txt");``).  Subdirectories are
automatically created when you attempt to create a file in a subdirectory,
and when the last file in a subdirectory is removed the subdirectory
itself is automatically deleted.  This is because there was no ``mkdir()``
method in the existing SPIFFS filesystem.

Unlike SPIFFS, the actual file descriptors are allocated as requested
by the application, so in low memory conditions you may not be able to
open new files.  Conversely, this also means that only file descriptors
used will actually take space on the heap.

Because there are directories, the ``openDir`` method behaves differently
than SPIFFS.  Whereas SPIFFS will return files in "subdirectories" when
you traverse a ``Dir::next()`` (because they really aren't subdirs but
simply files with "/"s in their names), LittleFS will only return files
in the specific subdirectory.  This mimics the POSIX behavior for
directory traversal most C programmers are used to.


Uploading files to file system
------------------------------

*ESP8266FS* is a tool which integrates into the Arduino IDE. It adds a
menu item to *Tools* menu for uploading the contents of sketch data
directory into ESP8266 flash file system.

**Warning**: Due to the move from the obsolete esptool-ck.exe to the
supported esptool.py upload tool, upgraders from pre 2.5.1 will need to
update the ESP8266FS tool referenced below to 0.4.0 or later.  Prior versions
will fail with a "esptool not found" error because they don't know how to
use esptool.py.

-  Download the tool: https://github.com/esp8266/arduino-esp8266fs-plugin/releases/download/0.4.0/ESP8266FS-0.4.0.zip
-  In your Arduino sketchbook directory, create ``tools`` directory if
   it doesn't exist yet
-  Unpack the tool into ``tools`` directory (the path will look like
   ``<home_dir>/Arduino/tools/ESP8266FS/tool/esp8266fs.jar``)
   If upgrading, overwrite the existing JAR file with the newer version.
-  Restart Arduino IDE
-  Open a sketch (or create a new one and save it)
-  Go to sketch directory (choose Sketch > Show Sketch Folder)
-  Create a directory named ``data`` and any files you want in the file
   system there
-  Make sure you have selected a board, port, and closed Serial Monitor
-  If your board requires you to press a button (or other action) to enter
   bootload mode for flashing a sketch, do that now.
-  Select Tools > ESP8266 Sketch Data Upload. This should start
   uploading the files into ESP8266 flash file system. When done, IDE
   status bar will display ``SPIFFS Image Uploaded`` message.

*ESP8266LittleFS* is the equivalent tool for LittleFS.

- Download the tool: https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases
- Install as above
- To upload a LittleFS filesystem use Tools > ESP8266 LittleFS Data Upload


File system object (SPIFFS/LittleFS)
------------------------------------

setConfig
~~~~~~~~~

.. code:: cpp

    SPIFFSConfig cfg;
    cfg.setAutoFormat(false);
    SPIFFS.setConfig(cfg);

This method allows you to configure the parameters of a filesystem
before mounting.  All filesystems have their own ``*Config`` (i.e.
``SDFSConfig`` or ``SPIFFSConfig`` with their custom set of options.
All filesystems allow explicitly enabling/disabling formatting when
mounts fail.  If you do not call this ``setConfig`` method before
perforing ``begin()``, you will get the filesystem's default
behavior and configuration. By default, SPIFFS will autoformat the
filesystem if it cannot mount it, while SDFS will not.

begin
~~~~~

.. code:: cpp

    SPIFFS.begin()
    or LittleFS.begin()

This method mounts file system. It must be called before any
other FS APIs are used. Returns *true* if file system was mounted
successfully, false otherwise.  With no options it will format SPIFFS
if it is unable to mount it on the first try.

Note that both methods will automatically format the filesystem
if one is not detected.  This means that if you attempt a
``SPIFFS.begin()`` on a LittleFS filesystem you will lose all data
on that filesystem, and vice-versa.

end
~~~

.. code:: cpp

    SPIFFS.end()
    or LittleFS.end()

This method unmounts the file system. Use this method before updating
the file system using OTA.

format
~~~~~~

.. code:: cpp

    SPIFFS.format()
    or LittleFS.format()

Formats the file system. May be called either before or after calling
``begin``. Returns *true* if formatting was successful.

open
~~~~

.. code:: cpp

    SPIFFS.open(path, mode)
    or LittleFS.open(path, mode)

Opens a file. ``path`` should be an absolute path starting with a slash
(e.g. ``/dir/filename.txt``). ``mode`` is a string specifying access
mode. It can be one of "r", "w", "a", "r+", "w+", "a+". Meaning of these
modes is the same as for ``fopen`` C function.

::

       r      Open text file for reading.  The stream is positioned at the
              beginning of the file.

       r+     Open for reading and writing.  The stream is positioned at the
              beginning of the file.

       w      Truncate file to zero length or create text file for writing.
              The stream is positioned at the beginning of the file.

       w+     Open for reading and writing.  The file is created if it does
              not exist, otherwise it is truncated.  The stream is
              positioned at the beginning of the file.

       a      Open for appending (writing at end of file).  The file is
              created if it does not exist.  The stream is positioned at the
              end of the file.

       a+     Open for reading and appending (writing at end of file).  The
              file is created if it does not exist.  The initial file
              position for reading is at the beginning of the file, but
              output is always appended to the end of the file.

Returns *File* object. To check whether the file was opened
successfully, use the boolean operator.

.. code:: cpp

    File f = SPIFFS.open("/f.txt", "w");
    if (!f) {
        Serial.println("file open failed");
    }

exists
~~~~~~

.. code:: cpp

    SPIFFS.exists(path)
    or LittleFS.exists(path)

Returns *true* if a file with given path exists, *false* otherwise.

mkdir
~~~~~

.. code:: cpp

    LittleFS.mkdir(path)

Returns *true* if the directory creation succeeded, *false* otherwise.

rmdir
~~~~~

.. code:: cpp

    LittleFS.rmdir(path)

Returns *true* if the directory was successfully removed, *false* otherwise.


openDir
~~~~~~~

.. code:: cpp

    SPIFFS.openDir(path)
    or LittleFS.openDir(path)

Opens a directory given its absolute path. Returns a *Dir* object.
Please note the previous discussion on the difference in behavior between
LittleFS and SPIFFS for this call.

remove
~~~~~~

.. code:: cpp

    SPIFFS.remove(path)
    or LittleFS.remove(path)

Deletes the file given its absolute path. Returns *true* if file was
deleted successfully.

rename
~~~~~~

.. code:: cpp

    SPIFFS.rename(pathFrom, pathTo)
    or LittleFS.rename(pathFrom, pathTo)

Renames file from ``pathFrom`` to ``pathTo``. Paths must be absolute.
Returns *true* if file was renamed successfully.

info
~~~~

.. code:: cpp

    FSInfo fs_info;
    SPIFFS.info(fs_info);
    or LittleFS.info(fs_info);

Fills `FSInfo structure <#filesystem-information-structure>`__ with
information about the file system. Returns ``true`` is successful,
``false`` otherwise.

Filesystem information structure
--------------------------------

.. code:: cpp

    struct FSInfo {
        size_t totalBytes;
        size_t usedBytes;
        size_t blockSize;
        size_t pageSize;
        size_t maxOpenFiles;
        size_t maxPathLength;
    };

This is the structure which may be filled using FS::info method. -
``totalBytes`` — total size of useful data on the file system -
``usedBytes`` — number of bytes used by files - ``blockSize`` — filesystem
block size - ``pageSize`` — filesystem logical page size - ``maxOpenFiles``
— max number of files which may be open simultaneously -
``maxPathLength`` — max file name length (including one byte for zero
termination)

gc
~~

.. code:: cpp

    SPIFFS.gc()

Only implemented in SPIFFS.  Performs a quick garbage collection operation on SPIFFS,
possibly making writes perform faster/better in the future.  On very full or very fragmented
filesystems, using this call can avoid or reduce issues where SPIFFS reports free space
but is unable to write additional data to a file.  See `this discussion
<https://github.com/esp8266/Arduino/pull/6340#discussion_r307042268>` for more info.

check
~~~~~

.. code:: cpp

    SPIFFS.begin();
    SPIFFS.check();

Only implemented in SPIFFS.  Performs an in-depth check of the filesystem metadata and
correct what is repairable.  Not normally needed, and not guaranteed to actually fix
anything should there be corruption.

Directory object (Dir)
----------------------

The purpose of *Dir* object is to iterate over files inside a directory.
It provides multiple access methods.

The following example shows how it should be used:

.. code:: cpp

    Dir dir = SPIFFS.openDir("/data");
    // or Dir dir = LittleFS.openDir("/data");
    while (dir.next()) {
        Serial.print(dir.fileName());
        if(dir.fileSize()) {
            File f = dir.openFile("r");
            Serial.println(f.size());
        }
    }

next
~~~~

Returns true while there are files in the directory to
iterate over. It must be called before calling ``fileName()``, ``fileSize()``,
and ``openFile()`` functions.

fileName
~~~~~~~~~

Returns the name of the current file pointed to
by the internal iterator.

fileSize
~~~~~~~~

Returns the size of the current file pointed to
by the internal iterator.

isFile
~~~~~~

Returns *true* if the current file pointed to by
the internal iterator is a File.

isDirectory
~~~~~~~~~~~

Returns *true* if the current file pointed to by
the internal iterator is a Directory.

openFile
~~~~~~~~

This method takes *mode* argument which has the same meaning as
for ``SPIFFS/LittleFS.open()`` function.

rewind
~~~~~~

Resets the internal pointer to the start of the directory.

File object
-----------

``SPIFFS/LittleFS.open()`` and ``dir.openFile()`` functions return a *File* object.
This object supports all the functions of *Stream*, so you can use
``readBytes``, ``findUntil``, ``parseInt``, ``println``, and all other
*Stream* methods.

There are also some functions which are specific to *File* object.

seek
~~~~

.. code:: cpp

    file.seek(offset, mode)

This function behaves like ``fseek`` C function. Depending on the value
of ``mode``, it moves current position in a file as follows:

-  if ``mode`` is ``SeekSet``, position is set to ``offset`` bytes from
   the beginning.
-  if ``mode`` is ``SeekCur``, current position is moved by ``offset``
   bytes.
-  if ``mode`` is ``SeekEnd``, position is set to ``offset`` bytes from
   the end of the file.

Returns *true* if position was set successfully.

position
~~~~~~~~

.. code:: cpp

    file.position()

Returns the current position inside the file, in bytes.

size
~~~~

.. code:: cpp

    file.size()

Returns file size, in bytes.

name
~~~~

.. code:: cpp

    String name = file.name();

Returns short (no-path) file name, as ``const char*``. Convert it to *String* for
storage.

fullName
~~~~~~~~

.. code:: cpp

    // Filesystem:
    //   testdir/
    //           file1
    Dir d = LittleFS.openDir("testdir/");
    File f = d.openFile("r");
    // f.name() == "file1", f.fullName() == "testdir/file1"

Returns the full path file name as a ``const char*``.

isFile
~~~~~~

.. code:: cpp

    bool amIAFile = file.isFile();

Returns *true* if this File points to a real file.

isDirectory
~~~~~~~~~~~

.. code:: cpp

    bool amIADir = file.isDir();

Returns *true* if this File points to a directory (used for emulation
of the SD.* interfaces with the ``openNextFile`` method).

close
~~~~~

.. code:: cpp

    file.close()

Close the file. No other operations should be performed on *File* object
after ``close`` function was called.

openNextFile  (compatibiity method, not recommended for new code)
~~~~~~~~~~~~

.. code:: cpp

    File root = LittleFS.open("/");
    File file1 = root.openNextFile();
    File files = root.openNextFile();

Opens the next file in the directory pointed to by the File.  Only valid
when ``File.isDirectory() == true``.

rewindDirectory  (compatibiity method, not recommended for new code)
~~~~~~~~~~~~~~~

.. code:: cpp

    File root = LittleFS.open("/");
    File file1 = root.openNextFile();
    file1.close();
    root.rewindDirectory();
    file1 = root.openNextFile(); // Opens first file in dir again

Resets the ``openNextFile`` pointer to the top of the directory.  Only
valid when ``File.isDirectory() == true``.
