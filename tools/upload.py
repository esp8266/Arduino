#!/usr/bin/env python3

# New Upload-Wrapper for Arduino esp8266
# 2021-08-08: G.N.: Totally rewritten to support (optional) uploading of sketch & filesystem in one step

import os
import sys
ToolsDir = os.path.dirname( os.path.realpath( __file__ ) ).replace( '\\', '/' ) + "/" # convert to UNIX format
try:
    sys.path.insert( 0, ToolsDir ) # ToolsDir
    from utilities import * # If this fails, we can't continue and will bomb below
except Exception as e:
    sys.stderr.write( '\nImport of utilities.py failed.\n...Is it not next to this %s tool?\n...Exception was: %s\n' % ( __file__, e )  )
    sys.exit( 2 )
import tempfile
from pathlib import Path

try:
    sys.path.insert( 0, ToolsDir + "pyserial" ) # Add pyserial dir to search path
    sys.path.insert( 0, ToolsDir + "esptool" ) # Add esptool dir to search path
    import esptool # If this fails, we can't continue and will bomb below
except Exception:
    Msg( '\npyserial or esptool directories not found next to this upload.py tool.' )
    sys.exit( 2 )

#Args: (original)
#  As defined in 'platform.txt' for 'tools.esptool.upload.pattern'  possible values (from boards.txt[.py])
#  |                                                                |
#  --chip esp8266 
#  --port "{serial.port}"                                           e.g.: /dev/ttyUSB0
#  --baud "{upload.speed}"                                          e.g.: 115200
#  "{upload.verbose}"                                               { '' | '--trace' }
#  {upload.erase_cmd}                                               { '' | 'erase_region "{build.rfcal_addr}" 0x4000' | 'erase_flash' }
#  {upload.resetmethod}                                             --before { default_reset | no_reset | no_reset_no_sync } --after { hard_reset | soft_reset }
#  write_flash 0x0 "{build.path}/{build.project_name}.bin"          write_flash <adr> <filePath for sketch-bin>

#Args: (combined firmware and filesystem)
#  As defined in 'platform.txt' for 'tools.esptool.upload.pattern'  possible values
#  |                                                                |
#  -up "{build.Upload}"                                             1-3
#  -fi "{build.Filesystem}"                                         0-2
#  --port "{serial.port}"                                           e.g.: /dev/ttyUSB0
#  --baud "{upload.speed}"                                          e.g.: 115200
#  {upload.resetmethod}                                             --before { default_reset | no_reset | no_reset_no_sync } --after { hard_reset | soft_reset }
#  --sk 0x0 "{build.path}/{build.project_name}.bin"                 <adr> <filePath for sketch-bin>
#  --fs {build.spiffs_start} "{build.path}/{build.project_name}"    <adr> <filePath for fs.bin, without ext>
#  {upload.erase_cmd}                                               { '' | 'erase_region "{build.rfcal_addr}" 0x4000' | 'erase_flash' }
#  "{upload.verbose}"                                               { '' | '--trace' }
# Remark:
#  {upload.erase_cmd} "{upload.verbose}" should be the last two parameters given in 'platform.txt'

def parse_args( argsIn ):
    parser = argparse.ArgumentParser( description = 'Upload-Wrapper for Arduino esp8266' )
    parser.add_argument( '-up', '--Upload', type = int, default = 1, help = '1: Sketch, 2: Filesystem, 3: Both' )
    parser.add_argument( '-fi', '--Filesystem', type = int, default = 0, help = '0: Off, 1: LitteFs, 3: SPIFFS' )
    parser.add_argument( '--port', type = str, required = True, help = 'path to serial device' )
    parser.add_argument( '--baud', type = str, required = True, help = 'baudrate' )
    parser.add_argument( '--before', type = str, required = True, help = 'before (default_reset/no_reset/no_reset_no_sync)' )
    parser.add_argument( '--after', type = str, required = True, help = 'after (hard_reset/soft_reset)' )
    parser.add_argument( '--sk', type = str, nargs=2, help = 'adr path_path_to_sketch_binary' )
    parser.add_argument( '--fs', type = str, nargs=2, help = 'adr path_path_to_fs_binary_without_ext' )
    parser.add_argument( 'erase_cmd', type = str, nargs='*', help = 'erase_cmd' )
    parser.add_argument( '--trace', action='store_true', help = 'verbose' )
    global Args
    Args = parser.parse_args( argsIn )


def main( argsIn = None ):
    """
    Main function for upload

    argsIn - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments.
    Arguments and their values need to be added as individual items to the list e.g. "-b 115200" thus becomes ['-b', '115200'].
    """

    parse_args( argsIn )
    
    if not os.path.exists( Args.port ):
      Msg( "Port: '%s', does not exist!\n...Upload aborted" % Args.port )
      sys.exit( 1 )

    # We silently replace the 921kbaud setting with 460k to enable backward
    # compatibility with the old esptool-ck.exe.  Esptool.py doesn't seem
    # work reliably at 921k, but is still significantly faster at 460kbaud.
    if Args.baud == "921600":
        Args.baud = "460800"
    esptoolArgs = [ '--chip', 'esp8266', '--port', Args.port, '--baud', Args.baud, '--before', Args.before, '--after', Args.after ]
    if Args.trace:
        esptoolArgs += [ '--trace' ]
    esptoolArgs += [ 'write_flash' ]

    EraseFilePath = ''
    filesUploaded = []
    try:
        if ( Args.Upload & 1 ) == 1:
            if len( Args.erase_cmd ) and Args.erase_cmd[ 0 ] == "erase_flash":
                esptoolArgs += [ '--erase-all' ]
            esptoolArgs += [ '--flash_size', 'detect', Args.sk[ 0 ], Args.sk[ 1 ] ]

            if len( Args.erase_cmd ) and Args.erase_cmd[ 0 ] == "erase_region":
                # Generate temporary empty (0xff) file
                f, EraseFilePath = tempfile.mkstemp()
                os.write( f, bytearray( [ 255 ] * int( Args.erase_cmd[ 2 ], 0 ) ) )
                os.close( f )
                esptoolArgs += [ Args.erase_cmd[ 1 ], EraseFilePath ]

            filesUploaded += [ Path( Args.sk[ 1 ] ).name ]

        if ( Args.Upload & 2 ) == 2:
            fsName = "%s.%s" % ( Args.fs[ 1 ], "littlefs" if Args.Filesystem == 1 else "spiffs" )
            if os.path.exists( fsName ):
                esptoolArgs += [ Args.fs[ 0 ], fsName ]
                filesUploaded += [ Path( fsName ).name ]

        Msg( "Uploading Binaries..." )
        #Debug( str( esptoolArgs ) )
        esptool.main( esptoolArgs )
        Msg( '...files uploaded: %s' % ", ".join( filesUploaded ) )
    finally:
        if len( EraseFilePath ):
            os.remove( EraseFilePath )
    
def main_():
    try:
        main()
    except RuntimeError as e:
        Msg( '\nA RuntimeError error occurred: %s' % e )
        sys.exit( 2 )

if __name__ == '__main__':
    main_()
