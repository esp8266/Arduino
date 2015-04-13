package processing.app.tools;

import org.apache.commons.exec.CommandLine;
import processing.app.BaseNoGui;
import processing.app.Platform;
import processing.app.helpers.OSUtils;

import java.io.File;

public class ArgumentsWithSpaceAwareCommandLine extends CommandLine {

  public ArgumentsWithSpaceAwareCommandLine(String executable) {
    super(executable);
  }

  public ArgumentsWithSpaceAwareCommandLine(File executable) {
    super(executable);
  }

  public ArgumentsWithSpaceAwareCommandLine(CommandLine other) {
    super(other);
  }

  @Override
  public CommandLine addArgument(String argument, boolean handleQuoting) {
    if (argument.contains(" ") && OSUtils.isWindows()) {
      argument = argument.replaceAll("\"", "").replaceAll("'", "");
    }

    return super.addArgument(argument, handleQuoting);
  }
}
