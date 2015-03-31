package cc.arduino.contributions;

import cc.arduino.contributions.packages.DownloadableContribution;

import java.util.Comparator;

public class DownloadableContributionBuiltInAtTheBottomComparator implements Comparator<DownloadableContribution> {
  @Override
  public int compare(DownloadableContribution p1, DownloadableContribution p2) {
    if (p1.isReadOnly() == p2.isReadOnly()) {
      return 0;
    }

    return p1.isReadOnly() ? 1 : -1;
  }
}
