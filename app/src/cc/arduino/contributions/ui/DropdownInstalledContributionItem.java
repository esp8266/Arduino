package cc.arduino.contributions.ui;

import cc.arduino.contributions.filters.InstalledPredicate;
import cc.arduino.contributions.packages.DownloadableContribution;
import cc.arduino.contributions.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownInstalledContributionItem implements DropdownItem<DownloadableContribution> {

  public String toString() {
    return _("Installed");
  }

  @Override
  public Predicate<DownloadableContribution> getFilterPredicate() {
    return new InstalledPredicate();
  }
}
