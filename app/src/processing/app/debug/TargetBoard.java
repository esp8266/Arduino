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

  /**
   * Check if the board has a sub menu.
   * 
   * @param menuId
   *          The menu ID to check
   * @return
   */
  public boolean hasMenu(String menuId) {
    return menuOptions.containsKey(menuId);
  }

  /**
   * Returns the options available on a specific menu
   * 
   * @param menuId
   *          The menu ID
   * @return
   */
  public PreferencesMap getMenuLabels(String menuId) {
    return menuOptions.get(menuId).topLevelMap();
  }

  /**
   * Returns the label of the specified option in the specified menu
   * 
   * @param menuId
   *          The menu ID
   * @param selectionId
   *          The option ID
   * @return
   */
  public String getMenuLabel(String menuId, String selectionId) {
    return getMenuLabels(menuId).get(selectionId);
  }

  /**
   * Returns the configuration parameters to override (as a PreferenceMap) when
   * the specified option in the specified menu is selected
   * 
   * @param menuId
   *          The menu ID
   * @param selectionId
   *          The option ID
   * @return
   */
  public PreferencesMap getMenuConfiguration(String menuId, String selectionId) {
    return menuOptions.get(menuId).subTree(selectionId);
  }
}
