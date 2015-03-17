package cc.arduino.contributions.filters;

import cc.arduino.contributions.packages.DownloadableContribution;
import com.google.common.base.Predicate;

public class InstalledPredicate implements Predicate<DownloadableContribution> {
  @Override
  public boolean apply(DownloadableContribution input) {
    return input.isInstalled();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DownloadableContribution;
  }

}
