package processing.app.helpers;

import static processing.app.I18n._;

import java.io.File;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.PreferencesData;
import processing.app.debug.TargetBoard;
import processing.app.debug.TargetPackage;
import processing.app.debug.TargetPlatform;
import processing.app.legacy.PApplet;

public class CommandlineParser {

  protected static enum ACTION { GUI, NOOP, VERIFY, UPLOAD, GET_PREF };

  private ACTION action = ACTION.GUI;
  private boolean doVerboseBuild = false;
  private boolean doVerboseUpload = false;
  private boolean doUseProgrammer = false;
  private boolean noUploadPort = false;
  private boolean forceSavePrefs = false;
  private String getPref = null;
  private List<String> filenames = new LinkedList<String>();

  public static CommandlineParser newCommandlineParser(String[] args) {
    return new CommandlineParser(args);
  }
  
  private CommandlineParser(String[] args) {
    parseArguments(args);
    checkAction();
  }
  
  private void parseArguments(String[] args) {
    // Map of possible actions and corresponding options
    final Map<String, ACTION> actions = new HashMap<String, ACTION>();
    actions.put("--verify", ACTION.VERIFY);
    actions.put("--upload", ACTION.UPLOAD);
    actions.put("--get-pref", ACTION.GET_PREF);

    // Check if any files were passed in on the command line
    for (int i = 0; i < args.length; i++) {
      ACTION a = actions.get(args[i]);
      if (a != null) {
        if (action != ACTION.GUI && action != ACTION.NOOP) {
          String[] valid = actions.keySet().toArray(new String[0]);
          String mess = I18n.format(_("Can only pass one of: {0}"), PApplet.join(valid, ", "));
          BaseNoGui.showError(null, mess, 3);
        }
        if (a == ACTION.GET_PREF) {
          i++;
          if (i >= args.length)
            BaseNoGui.showError(null, _("Argument required for --get-pref"), 3);
          getPref = args[i];
        }
        action = a;
        continue;
      }
      if (args[i].equals("--verbose") || args[i].equals("-v")) {
        doVerboseBuild = true;
        doVerboseUpload = true;
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--verbose-build")) {
        doVerboseBuild = true;
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--verbose-upload")) {
        doVerboseUpload = true;
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--useprogrammer")) {
        doUseProgrammer = true;
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--nouploadport")) {
        noUploadPort = true;
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--board")) {
        i++;
        if (i >= args.length)
          BaseNoGui.showError(null, _("Argument required for --board"), 3);
        processBoardArgument(args[i]);
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--port")) {
        i++;
        if (i >= args.length)
          BaseNoGui.showError(null, _("Argument required for --port"), 3);
        BaseNoGui.selectSerialPort(args[i]);
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--curdir")) {
        i++;
        if (i >= args.length)
          BaseNoGui.showError(null, _("Argument required for --curdir"), 3);
        // Argument should be already processed by Base.main(...)
        continue;
      }
      if (args[i].equals("--buildpath")) {
        i++;
        if (i >= args.length) {
          BaseNoGui.showError(null, "Argument required for --buildpath", 3);
        }
        File buildFolder = new File(args[i]);
        if (!buildFolder.exists()) {
          BaseNoGui.showError(null, "The build path doesn't exist", 3);
        }
        if (!buildFolder.isDirectory()) {
          BaseNoGui.showError(null, "The build path is not a folder", 3);
        }
        BaseNoGui.setBuildFolder(buildFolder);
        continue;
      }
      if (args[i].equals("--pref")) {
        i++;
        if (i >= args.length)
          BaseNoGui.showError(null, _("Argument required for --pref"), 3);
        processPrefArgument(args[i]);
        if (action == ACTION.GUI)
          action = ACTION.NOOP;
        continue;
      }
      if (args[i].equals("--save-prefs")) {
        forceSavePrefs = true;
        continue;
      }
      if (args[i].equals("--preferences-file")) {
        i++;
        if (i >= args.length)
          BaseNoGui.showError(null, _("Argument required for --preferences-file"), 3);
        // Argument should be already processed by Base.main(...)
        continue;
      }
      if (args[i].startsWith("--"))
        BaseNoGui.showError(null, I18n.format(_("unknown option: {0}"), args[i]), 3);

      filenames.add(args[i]);
    }
  }
  
  private void checkAction() {
    if ((action == ACTION.UPLOAD || action == ACTION.VERIFY) && filenames.size() != 1)
      BaseNoGui.showError(null, _("Must specify exactly one sketch file"), 3);

    if ((action == ACTION.NOOP || action == ACTION.GET_PREF) && filenames.size() != 0)
      BaseNoGui.showError(null, _("Cannot specify any sketch files"), 3);

    if ((action != ACTION.UPLOAD && action != ACTION.VERIFY) && (doVerboseBuild || doVerboseUpload))
      BaseNoGui.showError(null, _("--verbose, --verbose-upload and --verbose-build can only be used together with --verify or --upload"), 3);
  }

  private void processBoardArgument(String selectBoard) {
    // No board selected? Nothing to do
    if (selectBoard == null)
        return;

    String[] split = selectBoard.split(":", 4);

    if (split.length < 3) {
      BaseNoGui.showError(null, I18n.format(_("{0}: Invalid board name, it should be of the form \"package:arch:board\" or \"package:arch:board:options\""), selectBoard), 3);
    }

    TargetPackage targetPackage = BaseNoGui.getTargetPackage(split[0]);
    if (targetPackage == null) {
      BaseNoGui.showError(null, I18n.format(_("{0}: Unknown package"), split[0]), 3);
    }

    TargetPlatform targetPlatform = targetPackage.get(split[1]);
    if (targetPlatform == null) {
      BaseNoGui.showError(null, I18n.format(_("{0}: Unknown architecture"), split[1]), 3);
    }

    TargetBoard targetBoard = targetPlatform.getBoard(split[2]);
    if (targetBoard == null) {
      BaseNoGui.showError(null, I18n.format(_("{0}: Unknown board"), split[2]), 3);
    }

    BaseNoGui.selectBoard(targetBoard);

    if (split.length > 3) {
      String[] options = split[3].split(",");
      for (String option : options) {
        String[] keyValue = option.split("=", 2);

        if (keyValue.length != 2)
          BaseNoGui.showError(null, I18n.format(_("{0}: Invalid option, should be of the form \"name=value\""), option, targetBoard.getId()), 3);
        String key = keyValue[0].trim();
        String value = keyValue[1].trim();

        if (!targetBoard.hasMenu(key))
          BaseNoGui.showError(null, I18n.format(_("{0}: Invalid option for board \"{1}\""), key, targetBoard.getId()), 3);
        if (targetBoard.getMenuLabel(key, value) == null)
          BaseNoGui.showError(null, I18n.format(_("{0}: Invalid option for \"{1}\" option for board \"{2}\""), value, key, targetBoard.getId()), 3);

        PreferencesData.set("custom_" + key, targetBoard.getId() + "_" + value);
      }
    }
  }

  private void processPrefArgument(String arg) {
    String[] split = arg.split("=", 2);
    if (split.length != 2 || split[0].isEmpty())
      BaseNoGui.showError(null, I18n.format(_("{0}: Invalid argument to --pref, should be of the form \"pref=value\""), arg), 3);

    PreferencesData.set(split[0], split[1]);
  }

  public boolean isDoVerboseBuild() {
    return doVerboseBuild;
  }

  public boolean isDoVerboseUpload() {
    return doVerboseUpload;
  }

  public boolean isForceSavePrefs() {
    return forceSavePrefs;
  }

  public String getGetPref() {
    return getPref;
  }

  public List<String> getFilenames() {
    return filenames;
  }
  
  public boolean isGetPrefMode() {
    return action == ACTION.GET_PREF;
  }
  
  public boolean isGuiMode() {
    return action == ACTION.GUI;
  }
  
  public boolean isNoOpMode() {
    return action == ACTION.NOOP;
  }
  
  public boolean isUploadMode() {
    return action == ACTION.UPLOAD;
  }
  
  public boolean isVerifyMode() {
    return action == ACTION.VERIFY;
  }
  
  public boolean isVerifyOrUploadMode() {
    return isVerifyMode() || isUploadMode();
  }

  public boolean isDoUseProgrammer() {
    return doUseProgrammer;
  }

  public boolean isNoUploadPort() {
    return noUploadPort;
  }

}
