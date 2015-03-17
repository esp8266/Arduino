package cc.arduino.contributions.packages;

import cc.arduino.contributions.VersionComparator;

import java.util.Comparator;

public class ContributedPlatformComparator implements Comparator<ContributedPlatform> {

  @Override
  public int compare(ContributedPlatform lib1, ContributedPlatform lib2) {
    return VersionComparator.VERSION_COMPARATOR.compare(lib1.getVersion(), lib2.getVersion());
  }


}
