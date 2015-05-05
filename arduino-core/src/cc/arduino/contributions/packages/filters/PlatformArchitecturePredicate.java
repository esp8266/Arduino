package cc.arduino.contributions.packages.filters;

import cc.arduino.contributions.packages.ContributedPlatform;
import com.google.common.base.Predicate;

public class PlatformArchitecturePredicate implements Predicate<ContributedPlatform> {

  private final String platformArch;

  public PlatformArchitecturePredicate(String platformArch) {
    this.platformArch = platformArch;
  }

  @Override
  public boolean apply(ContributedPlatform contributedPlatform) {
    return platformArch.equals(contributedPlatform.getArchitecture());
  }

}
