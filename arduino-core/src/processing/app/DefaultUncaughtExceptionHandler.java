package processing.app;

public class DefaultUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {

  @Override
  public void uncaughtException(Thread t, Throwable e) {
    System.out.println(t);
    System.out.println(e);
  }

}
