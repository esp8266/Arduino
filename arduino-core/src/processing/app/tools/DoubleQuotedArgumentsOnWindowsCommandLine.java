package processing.app.tools;

import org.apache.commons.exec.CommandLine;
import processing.app.BaseNoGui;
import processing.app.Platform;
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
    if (argument.contains("\"") && OSUtils.isWindows()) {
      argument = argument.replace("\"", "\\\"");
    }

    return super.addArgument(argument, handleQuoting);
  }
}
