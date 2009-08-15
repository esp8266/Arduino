/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2008 Ben Fry and Casey Reas

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package processing.app;


import java.io.File;
import java.io.IOException;
import java.io.PrintStream;

import processing.core.PApplet;

import processing.app.debug.*;


/**
 * Class to handle running Processing from the command line.
 * <PRE>
 * --help               Show the help text.
 * 
 * --sketch=&lt;name&rt;      Specify the sketch folder (required)
 * --output=&lt;name&rt;      Specify the output folder (required and
 *                      cannot be the same as the sketch folder.)
 * 
 * --preprocess         Preprocess a sketch into .java files.
 * --build              Preprocess and compile a sketch into .class files.
 * --run                Preprocess, compile, and run a sketch.
 * --present            Preprocess, compile, and run a sketch full screen.
 * 
 * --export-applet      Export an applet.
 * --export-application Export an application.
 * --platform           Specify the platform (export to application only).
 *                      Should be one of 'windows', 'macosx', or 'linux'.
 * 
 * --preferences=&lt;file&rt; Specify a preferences file to use (optional).
 * </PRE>
 * 
 * To build the command line version, first build for your platform, 
 * then cd to processing/build/cmd and type 'dist.sh'. This will create a 
 * usable installation plus a zip file of the same. 
 * 
 * @author fry
 */
public class Commander implements RunnerListener {
  static final String helpArg = "--help";
  static final String preprocArg = "--preprocess";
  static final String buildArg = "--build";
  static final String runArg = "--run";
  static final String presentArg = "--present";
  static final String sketchArg = "--sketch=";
  static final String outputArg = "--output=";
  static final String exportAppletArg = "--export-applet";
  static final String exportApplicationArg = "--export-application";
  static final String platformArg = "--platform=";
  static final String preferencesArg = "--preferences=";

  static final int HELP = -1;
  static final int PREPROCESS = 0;
  static final int BUILD = 1;
  static final int RUN = 2;
  static final int PRESENT = 3;
  static final int EXPORT_APPLET = 4;
  static final int EXPORT_APPLICATION = 5;
  
  Sketch sketch;


  static public void main(String[] args) {
    // init the platform so that prefs and other native code is ready to go
    Base.initPlatform();
    // make sure a full JDK is installed
    Base.initRequirements();
    // run static initialization that grabs all the prefs
    //Preferences.init(null);
    // launch command line handler
    new Commander(args);
  }


  public Commander(String[] args) {
    String sketchFolder = null;
    String pdePath = null;  // path to the .pde file
    String outputPath = null;
    String preferencesPath = null;
    int platformIndex = PApplet.platform; // default to this platform
    int mode = HELP;

    for (String arg : args) {
      if (arg.length() == 0) {
        // ignore it, just the crappy shell script
        
      } else if (arg.equals(helpArg)) {
        // mode already set to HELP

      } else if (arg.equals(buildArg)) {
        mode = BUILD;

      } else if (arg.equals(runArg)) {
        mode = RUN;

      } else if (arg.equals(presentArg)) {
        mode = PRESENT;

      } else if (arg.equals(preprocArg)) {
        mode = PREPROCESS;

      } else if (arg.equals(exportAppletArg)) {
        mode = EXPORT_APPLET;

      } else if (arg.equals(exportApplicationArg)) {
        mode = EXPORT_APPLICATION;
        
      } else if (arg.startsWith(platformArg)) {
        String platformStr = arg.substring(platformArg.length());
        platformIndex = Base.getPlatformIndex(platformStr);
        if (platformIndex == -1) {
          complainAndQuit(platformStr + " should instead be " + 
                          "'windows', 'macosx', or 'linux'.");          
        }
      } else if (arg.startsWith(sketchArg)) {
        sketchFolder = arg.substring(sketchArg.length());
        File sketchy = new File(sketchFolder);
        File pdeFile = new File(sketchy, sketchy.getName() + ".pde");
        pdePath = pdeFile.getAbsolutePath();

      } else if (arg.startsWith(outputArg)) {
        outputPath = arg.substring(outputArg.length());
        
      } else {
        complainAndQuit("I don't know anything about " + arg + ".");
      }
    }

    if ((outputPath == null) &&
        (mode == PREPROCESS || mode == BUILD || 
         mode == RUN || mode == PRESENT)) {
      complainAndQuit("An output path must be specified when using " + 
                      preprocArg + ", " + buildArg + ", " + 
                      runArg + ", or " + presentArg + ".");
    }
    
    if (mode == HELP) {
      printCommandLine(System.out);
      System.exit(0);
    }

    // --present --platform=windows "--sketch=/Applications/Processing 0148/examples/Basics/Arrays/Array" --output=test-build
    
    File outputFolder = new File(outputPath);
    if (!outputFolder.exists()) {
      if (!outputFolder.mkdirs()) {
        complainAndQuit("Could not create the output folder.");
      }
    }

    // run static initialization that grabs all the prefs
    // (also pass in a prefs path if that was specified)
    Preferences.init(preferencesPath);

    if (sketchFolder == null) {
      complainAndQuit("No sketch path specified.");
      
    } else if (outputPath.equals(pdePath)) {
      complainAndQuit("The sketch path and output path cannot be identical.");
      
    } else if (!pdePath.toLowerCase().endsWith(".pde")) {
      complainAndQuit("Sketch path must point to the main .pde file.");
      
    } else {
      //Sketch sketch = null; 
      boolean success = false;

      try {
        sketch = new Sketch(null, pdePath);
        if (mode == PREPROCESS) {
          success = sketch.preprocess(outputPath) != null;

        } else if (mode == BUILD) {
          success = sketch.build(outputPath) != null;

        } else if (mode == RUN || mode == PRESENT) {
          String className = sketch.build(outputPath);
          if (className != null) {
            success = true;
            Runner runner = 
              new Runner(sketch, className, mode == PRESENT, this);
            runner.launch();

          } else {
            success = false;
          }
          
        } else if (mode == EXPORT_APPLET) {
          if (outputPath != null) {
            success = sketch.exportApplet(outputPath);
          } else {
            String target = sketchFolder + File.separatorChar + "applet";
            success = sketch.exportApplet(target);
          }
        } else if (mode == EXPORT_APPLICATION) {
          if (outputPath != null) {
            success = sketch.exportApplication(outputPath, platformIndex);
          } else {
            //String sketchFolder = 
            //  pdePath.substring(0, pdePath.lastIndexOf(File.separatorChar));
            outputPath = 
              sketchFolder + File.separatorChar + 
              "application." + Base.getPlatformName(platformIndex);
            success = sketch.exportApplication(outputPath, platformIndex);
          }
        }
        System.exit(success ? 0 : 1);

      } catch (RunnerException re) {
        statusError(re);
        
      } catch (IOException e) {
        e.printStackTrace();
        System.exit(1);
      }
    }
  }


  public void statusError(String message) {
    System.err.println(message);
  }


  public void statusError(Exception exception) {
    if (exception instanceof RunnerException) {
      RunnerException re = (RunnerException) exception;

      // format the runner exception like emacs
      //blah.java:2:10:2:13: Syntax Error: This is a big error message
      String filename = sketch.getCode(re.getCodeIndex()).getFileName();
      int line = re.getCodeLine();
      int column = re.getCodeColumn();
      if (column == -1) column = 0;
      // TODO if column not specified, should just select the whole line. 
      System.err.println(filename + ":" + 
                         line + ":" + column + ":" + 
                         line + ":" + column + ":" + " " + re.getMessage());
    } else {
      exception.printStackTrace();
    }
  }

  
  static void complainAndQuit(String lastWords) {
    printCommandLine(System.err);
    System.err.println(lastWords);
    System.exit(1);
  }


  static void printCommandLine(PrintStream out) {
    out.println("Processing " + Base.VERSION_NAME + " rocks the console.");
    out.println();
    out.println("--help               Show this help text.");
    out.println();
    out.println("--sketch=<name>      Specify the sketch folder (required)");
    out.println("--output=<name>      Specify the output folder (required and");
    out.println("                     cannot be the same as the sketch folder.)");
    out.println();
    out.println("--preprocess         Preprocess a sketch into .java files.");
    out.println("--build              Preprocess and compile a sketch into .class files.");
    out.println("--run                Preprocess, compile, and run a sketch.");
    out.println("--present            Preprocess, compile, and run a sketch full screen.");
    out.println();
    out.println("--export-applet      Export an applet.");
    out.println("--export-application Export an application.");
    out.println("--platform           Specify the platform (export to application only).");
    out.println("                     Should be one of 'windows', 'macosx', or 'linux'."); 
    out.println();
    out.println("--preferences=<file> Specify a preferences file to use (optional).");
  }
}