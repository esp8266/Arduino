package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.filters.BuiltInPredicate;
import cc.arduino.contributions.packages.DownloadableContribution;
import cc.arduino.contributions.ui.DropdownItem;
import com.google.common.base.Predicate;

import static processing.app.I18n._;

public class DropdownBuiltInLibrariesItem implements DropdownItem<DownloadableContribution> {

  public String toString() {
    return _("Built-in");
  }

  @Override
  public Predicate<DownloadableContribution> getFilterPredicate() {
    return new BuiltInPredicate();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof DropdownBuiltInLibrariesItem;
  }

}
