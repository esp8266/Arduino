package cc.arduino.contributions;

import processing.app.I18n;

import static processing.app.I18n._;

public class SignatureVerificationFailedException extends Exception {

  public SignatureVerificationFailedException(String filename) {
    super(I18n.format(_("{0} file signature verification failed"), filename));
  }

  public SignatureVerificationFailedException(String filename, Throwable cause) {
    super(I18n.format(_("{0} file signature verification failed"), filename), cause);
  }
}
