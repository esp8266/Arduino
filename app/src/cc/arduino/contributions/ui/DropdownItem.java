package cc.arduino.contributions.ui;

import com.google.common.base.Predicate;

public interface DropdownItem<T> {

  Predicate<T> getFilterPredicate();

}
