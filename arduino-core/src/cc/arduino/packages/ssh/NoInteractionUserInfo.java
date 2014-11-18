package cc.arduino.packages.ssh;

import com.jcraft.jsch.UserInfo;

public class NoInteractionUserInfo implements UserInfo {

  private final String password;

  public NoInteractionUserInfo(String password) {
    this.password = password;
  }

  public String getPassword() {
    return password;
  }

  public boolean promptYesNo(String str) {
    return true;
  }

  public String getPassphrase() {
    return password;
  }

  public boolean promptPassphrase(String message) {
    return true;
  }

  public boolean promptPassword(String message) {
    return true;
  }

  public void showMessage(String message) {
  }

}
