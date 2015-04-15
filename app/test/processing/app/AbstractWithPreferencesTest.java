package processing.app;

import org.junit.After;
import org.junit.Before;
import processing.app.helpers.FileUtils;

public abstract class AbstractWithPreferencesTest {

  @Before
  public void init() throws Exception {
    Base.initPlatform();
    Preferences.init(null);
    Theme.init();

    Base.untitledFolder = Base.createTempFolder("untitled");

  }

  @After
  public void cleanup() {
    FileUtils.recursiveDelete(Base.untitledFolder);
  }
}
