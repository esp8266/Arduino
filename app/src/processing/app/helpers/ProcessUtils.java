package processing.app.helpers;

import java.io.IOException;

import processing.app.Base;

public class ProcessUtils {

  public static Process exec(String[] command) throws IOException {
    // No problems on linux and mac
    if (!Base.isWindows()) {
      return Runtime.getRuntime().exec(command);
    }

    // Brutal hack to workaround windows command line parsing.
    // http://stackoverflow.com/questions/5969724/java-runtime-exec-fails-to-escape-characters-properly
    // http://msdn.microsoft.com/en-us/library/a1y7w461.aspx
    // http://bugs.sun.com/view_bug.do?bug_id=6468220
    // http://bugs.sun.com/view_bug.do?bug_id=6518827
    String[] cmdLine = new String[command.length];
    for (int i = 0; i < command.length; i++)
      cmdLine[i] = command[i].replace("\"", "\\\"");
    return Runtime.getRuntime().exec(cmdLine);
  }
}
