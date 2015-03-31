package processing.app.debug;

import java.util.Collection;
import java.util.Map;

public class TargetPackageStub implements TargetPackage {

  private final String id;

  public TargetPackageStub(String id) {
    this.id = id;
  }

  @Override
  public String getId() {
    return id;
  }

  @Override
  public Map<String, TargetPlatform> getPlatforms() {
    return null;
  }

  @Override
  public Collection<TargetPlatform> platforms() {
    return null;
  }

  @Override
  public TargetPlatform get(String platform) {
    return null;
  }

  @Override
  public boolean hasPlatform(TargetPlatform platform) {
    return false;
  }
}
