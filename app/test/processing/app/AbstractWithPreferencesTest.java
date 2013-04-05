package processing.app;

import org.junit.BeforeClass;

public abstract class AbstractWithPreferencesTest {

  @BeforeClass
  public static void setUp() throws Exception {
    Base.initPlatform();
    Preferences.init(null);
  }
}
