package cc.arduino.utils;

import java.util.Comparator;

public class ReverseComparator<T> implements Comparator<T> {

  private final Comparator<T> orig;

  public ReverseComparator(Comparator<T> orig) {
    this.orig = orig;
  }

  @Override
  public int compare(T t, T t1) {
    return -1 * orig.compare(t, t1);
  }
}
