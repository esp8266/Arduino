package cc.arduino;

public class DefaultUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {

  @Override
  public void uncaughtException(Thread t, Throwable e) {
    System.err.println(t);
    e.printStackTrace();
  }

}
