package processing.app;

import org.junit.BeforeClass;

public abstract class AbstractWithPreferencesTest {

  @BeforeClass
  public static void init() throws Exception {
    Base.initPlatform();
    Preferences.init(null);
  }
}
