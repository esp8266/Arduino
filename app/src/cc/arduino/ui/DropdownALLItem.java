package cc.arduino.ui;

import static processing.app.I18n._;

public class DropdownALLItem implements DropdownItem {

  @Override
  public String toString() {
    return _("All");
  }

  public String getItemValue() {
    return "";
  }
}
