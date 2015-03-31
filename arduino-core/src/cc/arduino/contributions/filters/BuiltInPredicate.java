package cc.arduino.contributions.filters;

import cc.arduino.contributions.packages.DownloadableContribution;
import com.google.common.base.Predicate;

public class BuiltInPredicate implements Predicate<DownloadableContribution> {

  @Override
  public boolean apply(DownloadableContribution input) {
    return input.isReadOnly();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof BuiltInPredicate;
  }
}
