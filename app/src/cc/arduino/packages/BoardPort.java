package cc.arduino.packages;

import processing.app.helpers.PreferencesMap;

public class BoardPort {

  private String address;
  private String protocol;
  private String boardName;
  private PreferencesMap prefs;

  public String getAddress() {
    return address;
  }

  public void setAddress(String address) {
    this.address = address;
  }

  public String getProtocol() {
    return protocol;
  }

  public void setProtocol(String protocol) {
    this.protocol = protocol;
  }

  public String getBoardName() {
    return boardName;
  }

  public void setBoardName(String boardName) {
    this.boardName = boardName;
  }

  public void setPrefs(PreferencesMap prefs) {
    this.prefs = prefs;
  }

  public PreferencesMap getPrefs() {
    return prefs;
  }

}
