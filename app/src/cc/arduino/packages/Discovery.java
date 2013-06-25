package cc.arduino.packages;

import java.util.List;

import processing.app.helpers.PreferencesMap;

public interface Discovery {

  /**
   * Set discovery preferences
   * 
   * @param options
   */
  public void setPreferences(PreferencesMap options);

  /**
   * Start discovery service
   * 
   * @throws Exception
   */
  public void start() throws Exception;

  /**
   * Stop discovery service
   */
  public void stop() throws Exception;

  /**
   * Return the list of discovered ports.
   * 
   * @return
   */
  public List<BoardPort> discovery();

}
