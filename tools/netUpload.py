#!/usr/bin/env python3

# Network-Upload-Wrapper
# 2021-08-08: G.N.: Written to support (optional) uploading of sketch & filesystem in one step

import os
import sys
ToolsDir = os.path.dirname( os.path.realpath( __file__ ) ).replace( '\\', '/' ) + "/" # convert to UNIX format
try:
    sys.path.insert( 0, ToolsDir ) # ToolsDir
    from utilities import * # If this fails, we can't continue and will bomb below
except Exception as e:
    sys.stderr.write( '\nImport of utilities.py failed.\n...Is it not next to this %s tool?\n...Exception was: %s\n' % ( __file__, e )  )
    sys.exit( 2 )

try:
    sys.path.insert( 0, ToolsDir ) # Add this dir to search path
    import espota # If this fails, we can't continue and will bomb below
except Exception:
    Msg( '\nespota not found next to this netUpload.py tool.' )
    sys.exit( 2 )

#Args: (original)
#  As defined in 'platform.txt' for 'tools.esptool.upload.network_pattern'
#  -i "{serial.port}"
#  -p "{network.port}"
#  "--auth={network.password}"
#  -f "{build.path}/{build.project_name}.bin"

#Args: (new)
#  As defined in 'platform.txt' for 'tools.esptool.upload.network_pattern'
#  |                                             possible values/meaning
#  |                                             |
#  -fi "{build.Filesystem}"                      0-4
#  -i "{serial.port}"
#  -p "{network.port}"
#  "--auth={network.password}"
#  --sk "{build.path}/{build.project_name}.bin"  <filePath for sketch-bin>
#  --fs "{build.path}/{build.project_name}"      <filePath for fs.bin, without ext>

def parse_args( argsIn ):
    parser = argparse.ArgumentParser( description = 'Network-Upload-Wrapper for Arduino esp8266' )
    parser.add_argument( '-fi', '--Filesystem', type = int, default = 0, help = '0: Off, 1: LitteFs: Create & Upload, 2: LitteFs: Create only, 3: SPIFFS: Create & Upload, 4: SPIFFS: Create only' )
    parser.add_argument( '-i', '--port', type = str, required = True, help = 'path to serial device' )
    parser.add_argument( '-p', '--netPort', type = str, required = True, help = 'network port' )
    parser.add_argument( '--auth', type = str, help = 'network password' )
    parser.add_argument( '--sk', type = str, required = True, help = 'path_path_to_sketch_binary' )
    parser.add_argument( '--fs', type = str, help = 'path_to_fs_binary_without_ext' )
    global Args
    Args = parser.parse_args( argsIn )

def main( argsIn = None ):
    """
    Main function for netUpload

    argsIn - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments.
    Arguments and their values need to be added as individual items to the list e.g. "-b 115200" thus becomes ['-b', '115200'].
    """

    parse_args( argsIn )

    if not os.path.exists( Args.port ):
      Msg( "Port: '%s', does not exist!\n...Upload aborted" % Args.port )
      sys.exit( 1 )
    
    baseArgs = [ '-i', Args.port, '-p', Args.netPort, '--auth', Args.auth ]
    
    sketchArgs = baseArgs + [ '-f', Args.sk ]

    Msg( "Uploading Binaries..." )

    #Debug( str( sketchArgs ) )
    esptool.main( sketchArgs )
    filesUploaded = [ Args.sk ]
    
    if Args.Filesystem == 1 or Args.Filesystem == 3:
        fsArgs = baseArgs + [ '-s', '-f' ]
        if Args.Filesystem == 1:
            fsArgs = baseArgs + [ "%s.littlefs" % Args.fs ]
        elif Args.Filesystem == 3:
            fsArgs = baseArgs + [ "%s.spiffs" % Args.fs ]

        #Debug( str( fsArgs ) )
        esptool.main( fsArgs )
        filesUploaded += [ Args.fs ]

    Msg( '...files uploaded: %s' % ", ".join( filesUploaded ) )
        
def main_():
    try:
        main()
    except RuntimeError as e:
        Msg( '\nA RuntimeError error occurred: %s' % e )
        sys.exit( 2 )

if __name__ == '__main__':
    main_()

