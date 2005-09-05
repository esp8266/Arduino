// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

// launcher.cpp : Defines the class behaviors for the application.
//

// The size of all of the strings was made sort of ambiguously large, since
// 1) nothing is hurt by allocating an extra few bytes temporarily and
// 2) if the user has a long path, and it gets copied five times over for the
// classpath, the program runs the risk of crashing. Bad bad.

#define JAVA_ARGS "-Xms128m -Xmx128m "
#define JAVA_MAIN_CLASS "processing.app.Base"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int STDCALL
WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
  // all these malloc statements... things may need to be larger.

  // what was passed to this application
  char *incoming_cmdline = (char *)malloc(strlen(lpCmd) * sizeof(char));
  strcpy (incoming_cmdline, lpCmd);

  // what gets put together to pass to jre
  char *outgoing_cmdline = (char *)malloc(16384 * sizeof(char));
        
  // prepend the args for -mx and -ms
  strcpy(outgoing_cmdline, JAVA_ARGS);

  // append the classpath and launcher.Application
  char *loaddir = (char *)malloc(MAX_PATH * sizeof(char));
  *loaddir = 0;

  GetModuleFileName(NULL, loaddir, MAX_PATH);
  // remove the application name
  *(strrchr(loaddir, '\\')) = '\0';

  char *cp = (char *)malloc(8 * strlen(loaddir) + 4096);


  // if this code looks shitty, that's because it is. people are 
  // likely to have the durndest things in their CLASSPATH and QTJAVA 
  // environment variables. mostly because installers often mangle 
  // them without the user knowing. so who knows where and when the
  // quotes will show up. this is a guess at dealing with the things, 
  // without spending a whole day to make it overly robust. [fry]

  
  

  // test to see if running with a java runtime nearby or not
  char *testpath = (char *)malloc(MAX_PATH * sizeof(char));
  *testpath = 0;
  strcpy(testpath, loaddir);
  strcat(testpath, "\\java\\bin\\java.exe");
  FILE *fp = fopen(testpath, "rb");
  int local_jre_installed = (fp != NULL);
  //char *rt_jar = (fp == NULL) ? "" : "java\\lib\\rt.jar;";
  if (fp != NULL) fclose(fp); // argh! this was probably causing trouble
  

  //MessageBox(NULL, local_jre_installed ? 
  //         "local jre installed" : "couldn't find jre", "p5", MB_OK);


  //const char *envClasspath = getenv("CLASSPATH");
  char *env_classpath = (char *)malloc(16384 * sizeof(char));

  // ignoring CLASSPATH for now, because it's not needed
  // and causes more trouble than it's worth [0060]
  env_classpath[0] = 0;

  /*
  // keep this code around since may be re-enabled later
  if (getenv("CLASSPATH") != NULL) {
    strcpy(env_classpath, getenv("CLASSPATH"));
    if (env_classpath[0] == '\"') {
      // starting quote in classpath.. yech
      env_classpath++;  // shitty.. i know..

      int len = strlen(env_classpath);
      if (env_classpath[len-1] == '\"') {
        env_classpath[len-1] = 0;
      } else {
        // a starting quote but no ending quote.. ugh
        // maybe throw an error
      }
    }
    int last = strlen(env_classpath);
    env_classpath[last++] = ';';
    env_classpath[last] = 0;
  } else {
    env_classpath[0] = 0;
  }
  */
/* WE DUNNO NEED QUICKTIME JAVA FOR ARDUINO
  char *qtjava_path = (char *)malloc(16384 * sizeof(char));
  qtjava_path[0] = 0;

  if (getenv("WINDIR") == NULL) {
    // uh-oh.. serious problem.. gonna have to report this
    // but hopefully WINDIR is set on win98 too

  } else {
    strcpy(qtjava_path, getenv("WINDIR"));
    strcat(qtjava_path, "\\SYSTEM32\\QTJava.zip");

    FILE *fp = fopen(qtjava_path, "rb");
    if (fp != NULL) {
      fclose(fp);  // found it, all set
      strcat(qtjava_path, ";"); // add path separator

    } else {
      strcpy(qtjava_path, getenv("WINDIR"));
      strcat(qtjava_path, "\\SYSTEM\\QTJava.zip");

      fp = fopen(qtjava_path, "rb");
      if (fp != NULL) {
        fclose(fp);  // found it, all set
        strcat(qtjava_path, ";"); // add path separator

      } else {
        // doesn't seem to be installed, which is a problem.
        // but the error will be reported by the pde
        qtjava_path[0] = 0;
      }
    }
  }
END WE DUNNO NEED QUICKTIME JAVA FOR ARDUINO*/  
  // NO! put quotes around contents of cp, because %s might have spaces in it.
  // don't put quotes in it, because it's setting the environment variable
  // for CLASSPATH, not being included on the command line. so setting the
  // env var it's ok to have spaces, and the quotes prevent 
  // javax.comm.properties from being found.
  sprintf(cp,	
          //"\""   // begin quote
          //"'"

          "%s"  // local jre or blank
          //"%s"  // qtjava path
          ""

          "%s\\lib;"
          "%s\\lib\\build;"
          "%s\\lib\\pde.jar;"
          //"%s\\lib\\core.jar;"
          "%s\\lib\\mrj.jar;"
          "%s\\lib\\RXTXcomm.jar;"
          "%s\\lib\\oro.jar;"
          "%s\\lib\\registry.jar;"
          "%s\\lib\\antlr.jar;"
          
          "%s",  // original CLASSPATH
          

          //"C:\\WINNT\\system32\\QTJava.zip;"  // worthless
          //"C:\\WINDOWS\\system32\\QTJava.zip;"

          //"\"",   // end quote
          //"'",
          //,

          // the first three %s args
          //local_jre_installed ? "java\\lib\\rt.jar;java\\lib\\jaws.jar;" : "", 
          local_jre_installed ? "java\\lib\\rt.jar;" : "", 
          //qtjava_path,
          loaddir, loaddir, loaddir, loaddir, 
          loaddir, loaddir, loaddir, loaddir, 
          env_classpath);

  //MessageBox(NULL, cp, "it's twoo! it's twoo!", MB_OK);

  if (!SetEnvironmentVariable("CLASSPATH", cp)) {
    MessageBox(NULL, "Could not set CLASSPATH environment variable",
               "Arduino Error", MB_OK);
    return 0;
  }
  
  

  // need to add the local jre to the path for 'java mode' in the env
  if (local_jre_installed) {
    
    char *env_path = (char *)malloc(strlen(getenv("PATH")) * sizeof(char));
    strcpy(env_path, getenv("PATH"));
    char *paf = (char *)malloc((strlen(env_path) + strlen(loaddir) + 32) * sizeof(char));
    sprintf(paf, "%s\\java\\bin;lib;%s", loaddir, env_path);

    if (!SetEnvironmentVariable("PATH", paf)) {
      MessageBox(NULL, "Could not set PATH environment variable",
                 "Arduino Error", MB_OK);
      return 0;
    }
    
	
  }

  //MessageBox(NULL, cp, "whaadddup", MB_OK);

  // add the name of the class to execute and a space before the next arg
  strcat(outgoing_cmdline, JAVA_MAIN_CLASS " ");

  // append additional incoming stuff (document names), if any
  strcat(outgoing_cmdline, incoming_cmdline);

  char *executable = (char *)malloc((strlen(loaddir) + 256) * sizeof(char));
  // loaddir is the name path to the current application

  //if (localJreInstalled) {
  if (local_jre_installed) {
    strcpy(executable, loaddir);
    // copy in the path for javaw, relative to launcher.exe
    strcat(executable, "\\java\\bin\\javaw.exe");
  } else {
    strcpy(executable, "javaw.exe");
  }

  SHELLEXECUTEINFO ShExecInfo;

  // set up the execution info
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = 0;
  ShExecInfo.hwnd = 0;
  ShExecInfo.lpVerb = "open";
  ShExecInfo.lpFile = executable;
  ShExecInfo.lpParameters = outgoing_cmdline;
  ShExecInfo.lpDirectory = loaddir;
  ShExecInfo.nShow = SW_SHOWNORMAL;
  ShExecInfo.hInstApp = NULL;

  if (!ShellExecuteEx(&ShExecInfo)) {
    MessageBox(NULL, "Error calling ShellExecuteEx()", 
               "Arduino Error", MB_OK);
    return 0;
  }

  if (reinterpret_cast<int>(ShExecInfo.hInstApp) <= 32) {

    // some type of error occurred
    switch (reinterpret_cast<int>(ShExecInfo.hInstApp)) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
	    MessageBox(NULL, "A required file could not be found. \n"
                 "You may need to install a Java runtime\n"
                 "or re-install Arduino.",
                 "Arduino Error", MB_OK);
	    break;
    case 0:
    case SE_ERR_OOM:
	    MessageBox(NULL, "Not enough memory or resources to run at"
                 " this time.", "Arduino Error", MB_OK);
	    
	    break;
    default:
	    MessageBox(NULL, "There is a problem with your installation.\n"
                 "If the problem persists, re-install the program.", 
                 "Arduino Error", MB_OK);
	    break;
    }
  }

  return 0;
}
