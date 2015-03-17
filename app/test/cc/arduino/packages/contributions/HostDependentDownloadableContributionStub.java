package cc.arduino.packages.contributions;

import cc.arduino.contributions.packages.HostDependentDownloadableContribution;

public class HostDependentDownloadableContributionStub extends HostDependentDownloadableContribution {
  @Override
  public String getHost() {
    return null;
  }

  @Override
  public String getUrl() {
    return null;
  }

  @Override
  public String getChecksum() {
    return null;
  }

  @Override
  public long getSize() {
    return 0;
  }

  @Override
  public String getArchiveFileName() {
    return null;
  }
}
