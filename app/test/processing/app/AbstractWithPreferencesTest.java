package processing.app;

import org.junit.Before;

public abstract class AbstractWithPreferencesTest {

  @Before
  public void init() throws Exception {
    Base.initPlatform();
    Preferences.init(null);
    Theme.init();

    Base.untitledFolder = Base.createTempFolder("untitled");
    Base.untitledFolder.deleteOnExit();

  }
}
