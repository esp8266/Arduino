package cc.arduino.contributions.ui;

import cc.arduino.contributions.filters.NoopPredicate;
import cc.arduino.contributions.packages.DownloadableContribution;
import cc.arduino.contributions.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownAllItem implements DropdownItem<DownloadableContribution> {

  public String toString() {
    return _("All");
  }

  @Override
  public Predicate<DownloadableContribution> getFilterPredicate() {
    return new NoopPredicate<DownloadableContribution>();
  }
}
