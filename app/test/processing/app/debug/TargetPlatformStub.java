package processing.app.debug;

import processing.app.helpers.PreferencesMap;

import java.io.File;
import java.util.Map;
import java.util.Set;

public class TargetPlatformStub implements TargetPlatform {

  private final String id;
  private final TargetPackage targetPackage;

  public TargetPlatformStub(String id, TargetPackage targetPackage) {
    this.id = id;
    this.targetPackage = targetPackage;
  }

  @Override
  public String getId() {
    return id;
  }

  @Override
  public File getFolder() {
    return null;
  }

  @Override
  public Map<String, TargetBoard> getBoards() {
    return null;
  }

  @Override
  public PreferencesMap getCustomMenus() {
    return null;
  }

  @Override
  public Set<String> getCustomMenuIds() {
    return null;
  }

  @Override
  public Map<String, PreferencesMap> getProgrammers() {
    return null;
  }

  @Override
  public PreferencesMap getProgrammer(String programmer) {
    return null;
  }

  @Override
  public PreferencesMap getTool(String tool) {
    return null;
  }

  @Override
  public PreferencesMap getPreferences() {
    return null;
  }

  @Override
  public TargetBoard getBoard(String boardId) {
    return null;
  }

  @Override
  public TargetPackage getContainerPackage() {
    return targetPackage;
  }
}
