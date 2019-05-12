# Arduino "class SD" shim wrapper

This is a simple wrapper class to replace the ancient Arduino SD.h
access method for SD cards.  It calls the underlying SDFS and the latest
SdFat lib to do all the work, and is now compatible with the rest of the
ESP8266 filesystem things.

-Earle F. Philhower, III
 <earlephilhower@yahoo.com>
