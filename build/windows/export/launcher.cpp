// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

// if this code looks shitty, that's because it is. people are likely to have
// the durndest things in their CLASSPATH and QTJAVA environment variables. 
// mostly because installers often mangle them without the user knowing. 
// so who knows where and when the quotes will show up. the code below is 
// based on a couple years of trial and error with processing releases.

// For revision 0102, a lot of changes were made to deal with stripping
// the quotes from the PATH, CLASSPATH, and QTJAVA environment variables.
// Any elements of the PATH and CLASSPATH that don't exist (whether files
// or directories) are also stripped out before being set.
// (<A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=112">Bug 112</A>)

// The size of all of the strings was made sort of ambiguously large, since
// 1) nothing is hurt by allocating an extra few bytes temporarily and
// 2) if the user has a long path, and it gets copied five times over for the
// CLASSPATH, the program runs the risk of crashing. Bad bad.

// TODO this code leaks memory all over the place because nothing has been
//      done to properly handle creation/deletion of new strings. 

// TODO switch to unicode versions of all methods in order to better support
//      running on non-English (non-Roman especially) versions of Windows.

#define ARGS_FILE_PATH "\\lib\\args.txt"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


void removeLineEndings(char *what);
char *scrubPath(char *incoming);
char *mallocChars(int count);
void removeQuotes(char *quoted);
void removeTrailingSlash(char *slashed);

int STDCALL
WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
  // command line that was passed to this application
  char *incoming_cmd_line = (char *)malloc(strlen(lpCmd + 1) * sizeof(char));
  strcpy(incoming_cmd_line, lpCmd);

  // get the full path to the application that was launched,
  // drop the app name but keep the path
  char *exe_directory = (char *)malloc(MAX_PATH * sizeof(char));
  //*exe_directory = 0;
  GetModuleFileName(NULL, exe_directory, MAX_PATH);
  // remove the application name
  *(strrchr(exe_directory, '\\')) = '\0';


  // open the file that contains the main class name and java args

  char *args_file_path = (char*) 
    malloc(strlen(exe_directory) * sizeof(char) + 
           strlen(ARGS_FILE_PATH) * sizeof(char) + 1);
  strcpy(args_file_path, exe_directory);
  strcat(args_file_path, ARGS_FILE_PATH);

  char java_args[512];
  char java_main_class[512];
  char jar_list[512];
  char *app_classpath = (char *)malloc(10 * strlen(exe_directory) + 4096);

  FILE *argsfile = fopen(args_file_path, "r");
  if (argsfile == NULL) {
    sprintf(app_classpath, 
            "This program is missing the \"lib\" folder, "
            "which should be located at\n%s", 
            exe_directory);
    MessageBox(NULL, app_classpath, "Folder Missing",  MB_OK);
    return 0;
    /*
    sprintf(app_classpath, 
            "%s\\lib;"
            "%s\\lib\\pde.jar;"
            "%s\\lib\\core.jar;"
            "%s\\lib\\jna.jar;"
            "%s\\lib\\ecj.jar;"
            "%s\\lib\\antlr.jar;",
            exe_directory, 
            exe_directory, exe_directory, 
            exe_directory, exe_directory, exe_directory);
    */
    
  } else {
    fgets(java_args, 511, argsfile);
    removeLineEndings(java_args);
    fgets(java_main_class, 511, argsfile);
    removeLineEndings(java_main_class);
    fgets(jar_list, 511, argsfile);
    removeLineEndings(jar_list);

    //MessageBox(NULL, java_args, "args",  MB_OK);
    //MessageBox(NULL, java_main_class, "class", MB_OK);
    //MessageBox(NULL, jar_list, "jarlist", MB_OK);

    app_classpath[0] = 0;
    char *jar = (char*) strtok(jar_list, ",");
    while (jar != NULL) {
      char entry[1024];
      sprintf(entry, "%s\\lib\\%s;", exe_directory, jar);
      strcat(app_classpath, entry);
      jar = (char*) strtok(NULL, ",");
    }
    fclose(argsfile);
  }

  //  

  char *cp = (char *)malloc(10 * strlen(exe_directory) + 4096);

  // test to see if running with a java runtime nearby or not
  char *testpath = (char *)malloc(MAX_PATH * sizeof(char));
  *testpath = 0;
  strcpy(testpath, exe_directory);
  strcat(testpath, "\\java\\bin\\java.exe");
  FILE *fp = fopen(testpath, "rb");
  int local_jre_installed = (fp != NULL);
  //char *rt_jar = (fp == NULL) ? "" : "java\\lib\\rt.jar;";
  if (fp != NULL) fclose(fp); // argh! this was probably causing trouble

  //const char *envClasspath = getenv("CLASSPATH");
  //char *env_classpath = (char *)malloc(16384 * sizeof(char));

  // ignoring CLASSPATH for now, because it's not needed
  // and causes more trouble than it's worth [0060]
  //env_classpath[0] = 0;

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

  char *qtjava_path = (char *)malloc(16384 * sizeof(char));
  qtjava_path[0] = 0;

  if (getenv("QTJAVA") != NULL) {
    //char *qtjava_temp = (char *)malloc(16384 * sizeof(char));
    strcpy(qtjava_path, getenv("QTJAVA"));
    removeQuotes(qtjava_path);

    /*
    //MessageBox(NULL, qtjava_temp, "QTJAVA", MB_OK);
    if (qtjava_temp[0] == '\"') {  // has quotes
      // remove quotes by subsetting string by two
      int qtjava_repaired_length = strlen(qtjava_temp) - 2;
      strncpy(qtjava_path, &qtjava_temp[1], qtjava_repaired_length);
      // terminate the string since strncpy ain't gonna do it
      qtjava_path[qtjava_repaired_length] = 0;
      //MessageBox(NULL, qtjava_path, "QTJAVA", MB_OK);
    } else {
      strcpy(qtjava_path, getenv("QTJAVA"));
    }
    */

    FILE *fp = fopen(qtjava_path, "rb");
    if (fp != NULL) {
      fclose(fp);  // found it, all set
      strcat(qtjava_path, ";"); // add path separator
      //MessageBox(NULL, "found 1", "msg", MB_OK);
      //MessageBox(NULL, qtjava_path, "QTJAVA after strcat", MB_OK);
    } else {
      qtjava_path[0] = 0; // not a valid path
    }
    //} else {
    //MessageBox(NULL, "no qtjava set", "mess", MB_OK);
  }

  if (qtjava_path[0] == 0) {  // not set yet
    //if (getenv("WINDIR") == NULL) {
    // uh-oh.. serious problem.. gonna have to report this
    // but hopefully WINDIR is set on win98 too
    //} else {

    strcpy(qtjava_path, getenv("WINDIR"));
    strcat(qtjava_path, "\\SYSTEM32\\QTJava.zip");

    FILE *fp = fopen(qtjava_path, "rb");
    if (fp != NULL) {
      fclose(fp);  // found it, all set
      strcat(qtjava_path, ";"); // add path separator
      //MessageBox(NULL, "found 2", "msg", MB_OK);
    } else {
      qtjava_path[0] = 0; // not a valid path      
    }
  }

  if (qtjava_path[0] == 0) {
    strcpy(qtjava_path, getenv("WINDIR"));
    strcat(qtjava_path, "\\SYSTEM\\QTJava.zip");
    
    fp = fopen(qtjava_path, "rb");
    if (fp != NULL) {
      fclose(fp);  // found it, all set
      strcat(qtjava_path, ";"); // add path separator
      //MessageBox(NULL, "found 3", "msg", MB_OK);
    } else {
      // doesn't seem to be installed, which is a problem.
      // but the error will be reported by the pde
      qtjava_path[0] = 0;
    }
  }

  // don't put quotes around contents of cp, even though %s might have 
  // spaces in it. don't put quotes in it, because it's setting the 
  // environment variable for CLASSPATH, not being included on the 
  // command line. so setting the env var it's ok to have spaces, 
  // and the quotes prevent javax.comm.properties from being found.

  strcpy(cp, app_classpath);
  if (local_jre_installed) {
    char *local_jre = mallocChars(64 + strlen(exe_directory) * 2);
    sprintf(local_jre, "%s\\java\\lib\\rt.jar;%s\\java\\lib\\tools.jar;", exe_directory, exe_directory);
    strcat(cp, local_jre);
  }
  strcat(cp, qtjava_path);

  char *clean_cp = scrubPath(cp);
  //if (!SetEnvironmentVariable("CLASSPATH", cp)) {
  if (!SetEnvironmentVariable("CLASSPATH", clean_cp)) {
    MessageBox(NULL, "Could not set CLASSPATH environment variable",
               "Processing Error", MB_OK);
    return 0;
  }

  //MessageBox(NULL, "2", "checking", MB_OK);

  //char *env_path = (char *)malloc(strlen(getenv("PATH")) * sizeof(char));

  int env_path_length = strlen(getenv("PATH"));
  char *env_path = mallocChars(env_path_length);
  strcpy(env_path, getenv("PATH"));
  char *clean_path;

  // need to add the local jre to the path for 'java mode' in the env
  if (local_jre_installed) {
    char *path_to_clean = 
      mallocChars(env_path_length + strlen(exe_directory) + 30);
    sprintf(path_to_clean, "%s\\java\\bin;%s", exe_directory, env_path);
    clean_path = scrubPath(path_to_clean);
  } else {
    clean_path = scrubPath(getenv("PATH"));
  }

  //MessageBox(NULL, clean_path, "after scrubbing PATH", MB_OK);
  //MessageBox(NULL, "3", "checking", MB_OK);

  if (!SetEnvironmentVariable("PATH", clean_path)) {
    MessageBox(NULL, "Could not set PATH environment variable",
               "Processing Error", MB_OK);
    return 0;
  }

  // what gets put together to pass to jre
  char *outgoing_cmd_line = (char *)malloc(16384 * sizeof(char));

  // prepend the args for -mx and -ms
  strcpy(outgoing_cmd_line, java_args);
  strcat(outgoing_cmd_line, " ");

  // add the name of the class to execute and a space before the next arg
  strcat(outgoing_cmd_line, java_main_class);
  strcat(outgoing_cmd_line, " ");

  // append additional incoming stuff (document names), if any
  strcat(outgoing_cmd_line, incoming_cmd_line);

  //MessageBox(NULL, outgoing_cmd_line, "cmd_line", MB_OK);

  char *executable = 
    (char *)malloc((strlen(exe_directory) + 256) * sizeof(char));
  // exe_directory is the name path to the current application

  if (local_jre_installed) {
    strcpy(executable, exe_directory);
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
  ShExecInfo.lpParameters = outgoing_cmd_line;
  ShExecInfo.lpDirectory = exe_directory;
  ShExecInfo.nShow = SW_SHOWNORMAL;
  ShExecInfo.hInstApp = NULL;

  if (!ShellExecuteEx(&ShExecInfo)) {
    MessageBox(NULL, "Error calling ShellExecuteEx()", 
               "Processing Error", MB_OK);
    return 0;
  }

  if (reinterpret_cast<int>(ShExecInfo.hInstApp) <= 32) {

    // some type of error occurred
    switch (reinterpret_cast<int>(ShExecInfo.hInstApp)) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
      MessageBox(NULL, "A required file could not be found. \n"
		 "You may need to install a Java runtime\n"
		 "or re-install Processing.",
		 "Processing Error", MB_OK);
      break;
    case 0:
    case SE_ERR_OOM:
      MessageBox(NULL, "Not enough memory or resources to run at"
                 " this time.", "Processing Error", MB_OK);
      
      break;
    default:
      MessageBox(NULL, "There is a problem with your installation.\n"
                 "If the problem persists, re-install the program.", 
                 "Processing Error", MB_OK);
      break;
    }
  }
  return 0;
}


void removeLineEndings(char *what) {
  int index = strlen(what) - 1;
  while (index >= 0) {
    if ((what[index] == 10) || (what[index] == 13)) {
      what[index] = 0;
      --index;
    } else {
      return;
    }
  }
}


// take a PATH environment variable, split on semicolons, 
// remove extraneous quotes, perhaps even make 8.3 syntax if necessary
char *scrubPath(char *incoming) {
  char *cleaned = mallocChars(strlen(incoming) * 2);

  int found_so_far = 0;
  char *p = (char*) strtok(incoming, ";");
  while (p != NULL) {
    char entry[1024];
    /* 
    if (*p == '\"') {
      // if this segment of the path contains quotes, remove them
      int fixed_length = strlen(p) - 2;
      strncpy(entry, &p[1], fixed_length);
      entry[fixed_length] = 0;
      //MessageBox(NULL, entry, "clipped", MB_OK);      

      // if it doesn't actually end with a quote, then the person 
      // is screwed anyway.. they can deal with that themselves
    } else {
      strcpy(entry, p);
    }
    */
    strcpy(entry, p);
    removeQuotes(entry);
    // a trailing slash will cause FindFirstFile to fail.. grr [0109]
    removeTrailingSlash(entry);
    //MessageBox(NULL, entry, "entry", MB_OK);

    // if this path doesn't exist, don't add it
    WIN32_FIND_DATA find_file_data;
    HANDLE hfind = FindFirstFile(entry, &find_file_data);
    if (hfind != INVALID_HANDLE_VALUE) {
      if (found_so_far) strcat(cleaned, ";");
      strcat(cleaned, entry);
      //MessageBox(NULL, cleaned, "cleaned so far", MB_OK);
      FindClose(hfind);
      found_so_far = 1; 
    //} else {
      //MessageBox(NULL, entry, "removing", MB_OK);
    }
    // grab the next entry
    p = (char*) strtok(NULL, ";");
  }
  //MessageBox(NULL, cleaned, "scrubPath", MB_OK);
  return cleaned;
}


// eventually make this handle unicode
char *mallocChars(int count) {
  // add one for the terminator
  char *outgoing = (char*) malloc(count * sizeof(char) + 1);
  outgoing[0] = 0;  // for safety
  return outgoing;
}


void removeQuotes(char *quoted) {
  int len = strlen(quoted);
  // remove quote at the front
  if (quoted[0] == '\"') {
    for (int i = 0; i < len - 1; i++) {
      quoted[i] = quoted[i+1];
    }
    len--;
    quoted[len] = 0;
  }
  // remove quote at the end
  if (len > 1) {
    if (quoted[len - 1] == '\"') {
      len--;
      quoted[len] = 0;
    }
  }
}


void removeTrailingSlash(char *slashed) {
  int len = strlen(slashed);  
  if (len > 1) {
    if (slashed[len - 1] == '\\') {
      len--;
      slashed[len] = 0;
    }
  }
}
