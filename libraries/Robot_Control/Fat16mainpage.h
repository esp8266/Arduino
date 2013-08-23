/* Arduino FAT16 Library
 * Copyright (C) 2008 by William Greiman
 *  
 * This file is part of the Arduino FAT16 Library
 *  
 * This Library is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with the Arduino Fat16 Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/**
\mainpage Arduino Fat16 Library
<CENTER>Copyright &copy; 2008 by William Greiman
</CENTER>

\section Intro Introduction
The Arduino Fat16 Library is a minimal implementation of the FAT16 file system
on standard SD flash memory cards.  Fat16 supports read, write, file
creation, deletion, and truncation. 

The Fat16 class only supports access to files in the root directory and only
supports short 8.3 names.  Directory time and date fields for creation
and modification can be maintained by providing a date/time callback
function \link Fat16::dateTimeCallback() dateTimeCallback()\endlink
or calling \link Fat16::timestamp() timestamp()\endlink.

Fat16 was designed to use the Arduino Print class which
allows files to be written with \link Print::print() print() \endlink and 
\link Print::println()  println()\endlink.

\section comment Bugs and Comments

If you wish to report bugs or have comments, send email to fat16lib@sbcglobal.net.


\section SDcard SD Cards

Arduinos access SD cards using the cards SPI protocol.  PCs, Macs, and
most consumer devices use the 4-bit parallel SD protocol.  A card that
functions well on A PC or Mac may not work well on the Arduino.

Most cards have good SPI read performance but cards vary widely in SPI
write performance.  Write performance is limited by how efficiently the
card manages internal erase/remapping operations.  The Arduino cannot
optimize writes to reduce erase operations because of its limit RAM.

SanDisk cards generally have good write performance.  They seem to have
more internal RAM buffering than other cards and therefore can limit
the number of flash erase operations that the Arduino forces due to its
limited RAM.

Some Dane-Elec cards have a write speed that is only 20% as fast as
a good SanDisk card.


\section Hardware Hardware Configuration
Fat16 was developed using an <A HREF = "http://www.adafruit.com/"> Adafruit Industries</A> 
<A HREF = "http://ladyada.net/make/gpsshield/modules.html"> GPS Shield</A>.

The hardware interface to the SD card should not use a resistor based level
shifter.  SdCard::init() sets the SPI bus frequency to 8 MHz which results in
signal rise times that are too slow for the edge detectors in many newer SD card
controllers when resistor voltage dividers are used.

The 5 to 3.3 V level shifter for 5 V arduinos should be IC based like the
74HC4050N based circuit shown in the file SdLevel.png.  The Adafruit Wave Shield
uses a 74AHC125N.  Gravitech sells SD and MicroSD Card Adapters based on the
74LCX245.

If you are using a resistor based level shifter and are having problems try
setting the SPI bus frequency to 4 MHz.  This can be done by using
card.init(true) to initialize the SD card.


\section Fat16Class Fat16 Usage

The class Fat16 is a minimal implementation of FAT16 on standard SD cards.
High Capacity SD cards, SDHC, are not supported. It should work on all
standard cards from 8MB to 2GB formatted with a FAT16 file system.

\note 
  The Arduino Print class uses character
at a time writes so it was necessary to use a \link Fat16::sync() sync() \endlink
function to control when data is written to the SD card.

\par
An application which writes to a file using \link Print::print() print()\endlink, 
\link Print::println() println() \endlink 
or \link Fat16::write write() \endlink must call \link Fat16::sync() sync() \endlink 
at the appropriate time to force data and directory information to be written 
to the SD Card.  Data and directory information are also written to the SD card
when \link Fat16::close() close() \endlink is called.

\par 
Applications must use care calling \link Fat16::sync() sync() \endlink
since 2048 bytes of I/O is required to update file and
directory information.  This includes writing the current data block, reading
the block that contains the directory entry for update, writing the directory
block back and reading back the current data block.

Fat16 only supports access to files in the root directory and only supports
short 8.3 names.

It is possible to open a file with two or more instances of Fat16.  A file may
be corrupted if data is written to the file by more than one instance of Fat16.

Short names are limited to 8 characters followed by an optional period (.)
and extension of up to 3 characters.  The characters may be any combination
of letters and digits.  The following special characters are also allowed:

$ % ' - _ @ ~ ` ! ( ) { } ^ # &

Short names are always converted to upper case and their original case
value is lost.  

Fat16 uses a slightly restricted form of short names.
Only printable ASCII characters are supported. No characters with code point
values greater than 127 are allowed.  Space is not allowed even though space 
was allowed in the API of early versions of DOS.

Fat16 has been optimized for The Arduino ATmega168.  Minimizing RAM use is the
highest priority goal followed by flash use and finally performance.  
Most SD cards only support 512 byte block write operations so a 512 byte
cache buffer is used by Fat16.  This is the main use of RAM.  A small
amount of RAM is used to store key volume and file information.
Flash memory usage can be controlled by selecting options in Fat16Config.h.

\section HowTo How to format SD Cards as FAT16 Volumes

Microsoft operating systems support removable media formatted with a 
Master Boot Record, MBR, or formatted as a super floppy with a FAT Boot Sector
in block zero.

Microsoft operating systems expect MBR formatted removable media 
to have only one partition. The first partition should be used.

Microsoft operating systems do not support partitioning SD flash cards.
If you erase an SD card with a program like KillDisk, Most versions of
Windows will format the card as a super floppy.

The best way to restore an SD card's MBR is to use SDFormatter 
which can be downloaded from:

http://www.sdcard.org/consumers/formatter/

SDFormatter does not have an option for FAT type so it may format
small cards as FAT12.

After the MBR is restored by SDFormatter you may need to reformat small
cards that have been formatted FAT12 to force the volume type to be FAT16. 

The FAT type, FAT12, FAT16, or FAT32, is determined by the count 
of clusters on the volume and nothing else.

Microsoft published the following code for determining FAT type:

\code
if (CountOfClusters < 4085) {
  // Volume is FAT12
}
else if (CountOfClusters < 65525) {
  // Volume is FAT16
}
else {
  // Volume is FAT32
}

\endcode
If you format a FAT volume with an OS utility , choose a cluster size that 
will result in:

4084 < CountOfClusters && CountOfClusters < 65525

The volume will then be FAT16.

If you are formatting an SD card on OS X or Linux, be sure to use the first 
partition. Format this partition with a cluster count in above range.

\section  References References

The Arduino site:

http://www.arduino.cc/

For more information about FAT file systems see:

http://www.microsoft.com/whdc/system/platform/firmware/fatgen.mspx

For information about using SD cards as SPI devices see:

http://www.sdcard.org/developers/tech/sdcard/pls/Simplified_Physical_Layer_Spec.pdf

The ATmega328 datasheet:

http://www.atmel.com/dyn/resources/prod_documents/doc8161.pdf
 

 */  