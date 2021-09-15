#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import gzip
import shutil
ToolsDir = os.path.dirname( os.path.realpath( __file__ ) ).replace( '\\', '/' ) + "/" # convert to UNIX format
try:
    sys.path.insert( 0, ToolsDir ) # ToolsDir
    from utilities import * # If this fails, we can't continue and will bomb below
except Exception as e:
    sys.stderr.write( '\nImport of utilities.py failed.\n...Is it not next to this %s tool?\n...Exception was: %s\n' % ( __file__, e )  )
    sys.exit( 2 )

def parse_args( argsIn ):
    parser = argparse.ArgumentParser( description = 'PostBuild for ESP8288' )
    parser.add_argument( '-fi', '--Filesystem', type = int, default = 0, help = '0: Off, 1: LitteFs, 3: SPIFFS' )
    parser.add_argument( '-ex', '--Export', type = int, default = 0, help = '0: Off, 1: .bin & .bin.signed, 2: Create & Export gzipped Binaries too' )
    parser.add_argument( '-na', '--name', type = str, required = True, help = 'Name of sketch (without extension)' )
    parser.add_argument( '-bp', '--buildPath', type = str, required = True, help = 'Path to temporary dir where sketch was build' )
    parser.add_argument( '-so', '--source', type = str, required = True, help = 'Path to sketch' )
    parser.add_argument( '-va', '--variant', type = str, required = True, help = 'Name of board' )
    parser.add_argument( '-ss', '--spiffs_start', type = str, help = 'build.spiffs_start' )
    parser.add_argument( '-se', '--spiffs_end', type = str, help = 'build.spiffs_end' )
    parser.add_argument( '-sp', '--spiffs_pagesize', type = str, default = '256', help = 'build.spiffs_pagesize' )
    parser.add_argument( '-sl', '--spiffs_blocksize', type = str, default = '4096', help = 'build.spiffs_blocksize' )
    global Args
    Args = parser.parse_args( argsIn )
    
def CountFilesInDir( dataDir ):
    fileCount = 0
    for root, dir, files in os.walk( dataDir ):
        for file in files:
            #!? Why Ony count files without '.' ??
            if file[ 0 ] != ".":
                fileCount += 1
    return fileCount

def CreateGzFile( dir, name ):
    """ gzip the file 'dir/name' and save the compressed file to 'dir/name.gz'. Return name of generated file as list. """
    outName = "%s.gz" % name
    outPath = os.path.join( dir, outName )
    inPath = os.path.join( dir, name )
    with open( inPath, 'rb' ) as inFile:
        with gzip.open( outPath, 'wb' ) as outFile:
            shutil.copyfileobj( inFile, outFile )
    if not os.path.exists( outPath ):
        raise FatalError( "gzip: file '%s' not created" % outName )
    return [ outName ]

def CreateSignedFile( dir, name ):
    """ Signs the file 'dir/name' with the private key (defined by global var 'SigningPrivKeyPath').
        Save the signed binary to 'dir/name.signed'. Return name of generated file as list. """
    inFilePath = os.path.join( dir, name )
    try:
        with open( inFilePath, "rb" ) as b:
            inFileData = b.read()
            signcmd = [ 'openssl', 'dgst', '-sha256', '-sign', SigningPrivKeyPath ]
            proc = subprocess.Popen( signcmd, stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.PIPE )
            signout, signerr = proc.communicate( input = inFileData )
            if proc.returncode:
                raise FatalError( "OpenSSL returned an error signing the binary: %d\nSTDERR: %d" % ( proc.returncode, signerr ) )
            outName = "%s.signed" % name
            with open( os.path.join( dir, outName ), "wb" ) as out:
                out.write( inFileData )
                out.write( signout )
                out.write( b'\x00\x01\x00\x00' )
            return [ outName ]
    except Exception as e:
        raise FatalError( "Exception while signing '%s': %s" % ( name, e ) )
    return []

def RemoveFs( fsName ):
    """ Removes the (possible existing from prev. builds) given fs from {build.path}
        This enshures, that (at upload-stage) we don't use the wrong fs created formerly.
        fsName = "littlefs" "spiffs"           used for naming the binary to remove. """
    imageName = "./%s.%s" % ( Args.name, fsName )
    if os.path.exists( imageName ):
        os.remove( imageName )

def CreateFs( fsName, mkFsName ):
    """ Creates the given fs from all files inside subdir 'data' of the sketch. 
        fsName = "littlefs" "spiffs"           used for naming the generated binary.
        mkFsName = "mklittlefs" | "mkspiffs"   the name of the programm that generates the fs """
    mkFsPath = Which( mkFsName, os.path.join( ToolsDir, mkFsName ) )
    if mkFsPath is None:
        mkFsPath = Which( "%s.exe" % mkFsName, os.path.join( ToolsDir, mkFsName ) )
    res = []
    dataDir = "%s/data" % ( Args.source )
    if len( Args.spiffs_start ) == 0 or len( Args.spiffs_end ) == 0:
        raise ProcessError( "CreateFs: %s Not Defined for this board" % fsName )
    if mkFsPath is None or not os.path.exists( mkFsPath ):
        raise ProcessError( "CreateFs: %s dir does not exist: %s" % ( mkFsName, mkFsPath ) )
    if not os.path.exists( dataDir ):
        raise ProcessError( "CreateFs: data dir does not exist: %s" % dataDir )
    if not os.path.isdir( dataDir ):
        raise ProcessError( "CreateFs: data dir is not a directory: %s" % dataDir )
    FilesInDir = CountFilesInDir( dataDir )
    if FilesInDir == 0:
        if not ConfirmDialog( "%s Create" % fsName, "No files have been found in your data folder!\nAre you sure you want to create an empty %s image?" % fsName ):
            raise ProcessError( "Canceled by user" )
    os.makedirs( DstDir, exist_ok = True )
    imageName = "%s.%s" % ( Args.name, fsName )
    imagePath = os.path.join( Args.source, "bin", Args.variant, imageName )
    cmd = "\"%s\" -c \"%s\" -p %s -b %s -s %d \"%s\"" % ( mkFsPath, dataDir, Args.spiffs_pagesize, Args.spiffs_blocksize, IntValFromStr( Args.spiffs_end ) - IntValFromStr( Args.spiffs_start ), imagePath )
    if os.system( cmd ) != 0 or not os.path.exists( imagePath ):
        raise FatalError( "%s: Creating failed" % fsName )
    res += [ imageName ]

    # fs is created inside subdir "bin/{build.variant}/" of sketch folder.
    # That means there is no need for doing an (extra) "export".
    # But unforunally during "upload" the variable {build.source.path} was not set.
    # So, for supporting upload of fs, we copy the generated file back into {build.path}
    shutil.copyfile( imagePath, "./%s" % imageName )

    signing = Signing and os.path.exists( os.path.abspath( Args.name + ".bin.signed" ) )
    if signing:
        res += CreateSignedFile( DstDir, imageName )
    if Args.Export == 2:
        res += CreateGzFile( DstDir, imageName )
        if signing:
            res += CreateSignedFile( DstDir, "%s.%s.gz" % ( Args.name, fsName ) )
    return res

def ProcessFilesystem_():
    if Args.spiffs_start is None or Args.spiffs_end is None:
        raise ProcessError( "'spiffs_start' and/or 'spiffs_end' missing" )
    if Args.Filesystem == 1:
        RemoveFs( "spiffs" )
        return CreateFs( "littlefs", "mklittlefs" )
    elif Args.Filesystem == 2:
        RemoveFs( "littlefs" )
        return CreateFs( "spiffs", "mkspiffs" )

def ProcessExport_():
    res = []
    os.makedirs( DstDir, exist_ok = True )
    # Copy Binaries
    res += CopyToDir( Args.name, "bin", DstDir )
    res += CopyToDir( Args.name, "bin.signed", DstDir )
    if Args.Export == 2:
        # Create & Export gzipped Binaries too
        res += CreateGzFile( DstDir, "%s.bin" % Args.name )
        if os.path.exists( os.path.abspath( os.path.join( DstDir, "%s.bin.signed" % Args.name ) ) ):
            res += CreateSignedFile( DstDir, "%s.bin.gz" % Args.name )
    return res

def ProcessFilesystem():
    Msg( "Creating Filesystem..." )
    try:
        filesCreated = ProcessFilesystem_()
        Msg( '...files generated: %s' % ", ".join( filesCreated ) )
    except ProcessError as e:
        Msg( '...aborted: %s' % e )
        try:
            WarningDialog( "Filesystem Create aborted", e )
        except ProcessError:
            pass

def ProcessExport():
    Msg( "Exporting Binaries..." )
    filesExported = ProcessExport_()
    Msg( '...files exported: %s' % ", ".join( filesExported ) )

def main( argsIn = None ):
    """
    Main function for postbuild

    argsIn - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments.
    Arguments and their values need to be added as individual items to the list e.g. "-b 115200" thus becomes ['-b', '115200'].
    """

    #Debug( "Python-Path: %s" % sys.executable ) 
    parse_args( argsIn )
    #Debug( "Posbuild: Args: %s" % ", ".join( sys.argv[ 1: ] ) )
    
    #!! For unknown reason using/concatinig Args.buildPath with "/" (or enything else) will result in an empthy string ???
    #if You uncomment the following two lines You will see something like:
    # Args.buildPath: /buildPath/arduino_build_572904.
    # Args.buildPath/: .
    #Debug( "Args.buildPath: %s." % Args.buildPath )
    #Debug( "Args.buildPath/: %s/." % Args.buildPath + '/' )

    # Workaround: cd to the build/temp path
    cwd = os.getcwd()
    os.chdir( Args.buildPath )
    Msg( 'Build-Dir: %s\n' % Args.buildPath )

    #!	 Even this will not give the correct path !
    #Debug( "abspath: %s" % os.path.abspath( "./" ) )
    #Debug( "abspath2: %s" % os.path.abspath( "./" ) + "/"  )
    
    global SigningPrivKeyPath, Signing, DstDir
    try:
        SigningPrivKeyPath = os.path.abspath( "%s/private.key" % Args.source )
        Signing = os.path.isfile( SigningPrivKeyPath )
        DstDir = os.path.join( Args.source, "bin", Args.variant )
        if Args.Filesystem == 0:
            RemoveFs( "littlefs" )
            RemoveFs( "spiffs" )
        else:
            ProcessFilesystem()
        if Args.Export != 0:
            ProcessExport()
    finally:
        # restore working dir    
        os.chdir( cwd )

def main_():
    try:
        main()
    except FatalError as e:
        Msg( '\nA fatal error occurred: %s\n' % e )
        sys.exit( 1 )
    except RuntimeError as e:
        Msg( '\nA RuntimeError error occurred: %s' % e )
        sys.exit( 2 )

if __name__ == '__main__':
    main_()
