package processing.app;

import cc.arduino.files.DeleteFilesOnShutdown;
import org.junit.Before;

public abstract class AbstractWithPreferencesTest {

  @Before
  public void init() throws Exception {
    Runtime.getRuntime().addShutdownHook(new Thread(DeleteFilesOnShutdown.INSTANCE));
    Base.initPlatform();
    Preferences.init(null);
    Theme.init();

    Base.untitledFolder = Base.createTempFolder("untitled");
    DeleteFilesOnShutdown.add(Base.untitledFolder);
  }

}
