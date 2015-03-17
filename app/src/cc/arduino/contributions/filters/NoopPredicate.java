package cc.arduino.contributions.filters;

import com.google.common.base.Predicate;

public class NoopPredicate<T> implements Predicate<T> {

  @Override
  public boolean apply(T input) {
    return true;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof NoopPredicate;
  }


}
