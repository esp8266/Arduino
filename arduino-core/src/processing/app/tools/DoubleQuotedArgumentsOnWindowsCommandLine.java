package processing.app.tools;

import org.apache.commons.exec.CommandLine;
import processing.app.helpers.OSUtils;

import java.io.File;

public class DoubleQuotedArgumentsOnWindowsCommandLine extends CommandLine {

  public DoubleQuotedArgumentsOnWindowsCommandLine(String executable) {
    super(executable);
  }

  public DoubleQuotedArgumentsOnWindowsCommandLine(File executable) {
    super(executable);
  }

  public DoubleQuotedArgumentsOnWindowsCommandLine(CommandLine other) {
    super(other);
  }

  @Override
  public CommandLine addArgument(String argument, boolean handleQuoting) {
    // Brutal hack to workaround windows command line parsing.
    // http://stackoverflow.com/questions/5969724/java-runtime-exec-fails-to-escape-characters-properly
    // http://msdn.microsoft.com/en-us/library/a1y7w461.aspx
    // http://bugs.sun.com/view_bug.do?bug_id=6468220
    // http://bugs.sun.com/view_bug.do?bug_id=6518827
    if (argument.contains("\"") && OSUtils.isWindows()) {
      argument = argument.replace("\"", "\\\"");
    }

    return super.addArgument(argument, handleQuoting);
  }
}
