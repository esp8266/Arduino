package processing.app.debug;

import processing.app.helpers.PreferencesMap;

public class TargetBoard {

  String id;
  PreferencesMap prefs;

  /**
   * Create a TargetBoard based on preferences passed as argument.
   * 
   * @param _prefs
   * @return
   */
  public TargetBoard(String _id, PreferencesMap _prefs) {
    id = _id;
    prefs = new PreferencesMap(_prefs);
  }

  /**
   * Get the name of the board.
   * 
   * @return
   */
  public String getName() {
    return prefs.get("name");
  }

  /**
   * Get the identifier of the board
   * 
   * @return
   */
  public String getId() {
    return id;
  }

  /**
   * Get the full preferences map of the board with a given identifier
   * 
   * @return
   */
  public PreferencesMap getPreferences() {
    return prefs;
  }
}
