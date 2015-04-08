package cc.arduino.contributions;

import cc.arduino.contributions.packages.DownloadableContribution;

import java.util.Comparator;

public class DownloadableContributionVersionComparator implements Comparator<DownloadableContribution> {

  private final VersionComparator versionComparator;

  public DownloadableContributionVersionComparator() {
    versionComparator = new VersionComparator();
  }

  @Override
  public int compare(DownloadableContribution lib1, DownloadableContribution lib2) {
    return versionComparator.compare(lib1.getParsedVersion(), lib2.getParsedVersion());
  }


}
