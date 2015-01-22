package processing.app;

import org.junit.After;
import org.junit.Assume;
import org.junit.Before;
import org.junit.Test;

import processing.app.debug.TargetBoard;
import static org.junit.Assert.assertNotEquals;

public class DefaultTargetTest extends AbstractWithPreferencesTest {

  private String oldBoardID;

  @Before
  public void saveBoardFromPreferences() throws Exception {
    oldBoardID = Preferences.get("board");
  }

  @After
  public void restoreBoardIntoPreferences() throws Exception {
    Preferences.set("board", oldBoardID);
    Preferences.save();
  }

  @Test
  public void testDefaultTarget() throws Exception {
    Preferences.set("board", "unreal_board");

    // should not raise an exception
    new Base(new String[0]);

    // skip test if no target platforms are available
    Assume.assumeNotNull(BaseNoGui.getTargetPlatform());
    
    TargetBoard targetBoard = BaseNoGui.getTargetBoard();
    assertNotEquals("unreal_board", targetBoard.getId());
  }
}
