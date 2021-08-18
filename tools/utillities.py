#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import platform
import argparse
import shutil
import subprocess

PyautoguiAvailable = True
try:
    import pyautogui
except:
    PyautoguiAvailable = False


class FatalError( Exception ):
    def __init__( self, msg ):
        super().__init__( msg )

class ProcessError( Exception ):
    def __init__( self, msg ):
        super().__init__( msg )

def Msg( txt ):
    sys.stderr.write( txt + "\n" )

def Debug( txt ):
    sys.stderr.write( txt + "\n" )

def Which_Chdir_( prg, dir ):
    cwd = os.getcwd()
    try:
        os.chdir( dir )
    except:
        return None
    res = None
    try:
        res = shutil.which( "./%s" % prg )
        if res is not None:
            res = os.path.abspath( os.path.join( os.getcwd(), prg ) )
    finally:
        os.chdir( cwd )
    return res

def Which( prg, dir = None ):
    """ returns full path to executable 'prg'.
        Takes care of OS, on Windows searches for <prg>.exe instead of <prg>.
        if <dir> is given prefers prg inside this dir over others"""
    if platform.system() == "Windows":
        prg = "%s.exe" % prg
    res = None
    if dir is not None:
        res = Which_Chdir_( prg, dir )
    if res is None:
        res = shutil.which( prg )
    return res

def CopyToDir( name, ext, dstDir ):
    """ if file './name.ext' exist, copy it to 'dstDir/name.ext'. Return name of copied file as list. """
    fileName = "%s.%s" % ( name, ext )
    srcPath = os.path.abspath( fileName )
    if not os.path.exists( srcPath ):
        return []
    shutil.copyfile( srcPath, os.path.join( dstDir, fileName ) )
    return [ fileName ]

def IntValFromStr( strVal ):
    """ Convert string into integer, can be used with decimal value '123' or hex '0xabc'. """
    return int( strVal.strip(), 0 )

def RemoveIno( path ):
    """ Returns the given path with extension ".ino" removed (if exist) """
    if path[ -4: ] == ".ino":
        path = name[ 0:-4 ]
    return path
    
def ConfirmDialog( title, text ):
    """ return -1 if tkinter not installed, 0 if aswer was no, 1 if answer was yes """
    try:
        import tkinter
        from tkinter import messagebox
    except:
        return -1
    rootWin = tkinter.Tk() # Create the object
    rootWin.overrideredirect( 1 ) # Avoid it appearing and then disappearing quickly
    #rootWin.iconbitmap("PythonIcon.ico") # Set an icon (this is optional - must be in a .ico format)
    rootWin.withdraw() # Hide the window as we do not want to see this one
    return 1 if messagebox.askyesno( title, text, parent = rootWin ) else 0

