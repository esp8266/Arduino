package cc.arduino.contributions.filters;

import cc.arduino.contributions.packages.DownloadableContribution;
import com.google.common.base.Predicate;

public class DownloadableContributionWithVersionPredicate implements Predicate<DownloadableContribution> {
  private final String version;

  public DownloadableContributionWithVersionPredicate(String version) {
    this.version = version;
  }

  @Override
  public boolean apply(DownloadableContribution contribution) {
    return version.equals(contribution.getParsedVersion());
  }

}
