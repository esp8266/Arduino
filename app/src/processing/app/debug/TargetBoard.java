package processing.app.debug;

import java.util.LinkedHashMap;
import java.util.Map;

import processing.app.helpers.PreferencesMap;

public class TargetBoard {

  private String id;
  private PreferencesMap prefs;
  private Map<String, PreferencesMap> menuOptions = new LinkedHashMap<String, PreferencesMap>();

  /**
   * Create a TargetBoard based on preferences passed as argument.
   * 
   * @param _prefs
   * @return
   */
  public TargetBoard(String _id, PreferencesMap _prefs) {
    id = _id;
    prefs = new PreferencesMap(_prefs);

    PreferencesMap menus = prefs.firstLevelMap().get("menu");
    if (menus != null)
      menuOptions = menus.firstLevelMap();
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

  public void setMenuOptions(String menuId, PreferencesMap _menuOptions) {
    menuOptions.put(menuId, _menuOptions);
  }

  public PreferencesMap getMenuOptions(String menuId) {
    return menuOptions.get(menuId);
  }

  public boolean hasMenuOptions(String menuId) {
    return menuOptions.containsKey(menuId);
  }
}
