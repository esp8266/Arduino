package processing.app.debug;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.methods.multipart.FilePart;
import org.apache.commons.httpclient.methods.multipart.MultipartRequestEntity;
import org.apache.commons.httpclient.methods.multipart.Part;
import processing.app.Preferences;
import processing.app.SerialException;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static processing.app.I18n._;

public class HttpUploader extends Uploader {

  private static final Pattern IPV4_ADDRESS = Pattern.compile("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");

  private final HttpClient client;
  private final String ipAddress;

  public HttpUploader(String port) {
    this.client = new HttpClient();
    Matcher matcher = IPV4_ADDRESS.matcher(port);
    if (!matcher.find()) {
      throw new IllegalArgumentException(port);
    }
    this.ipAddress = matcher.group(1);
  }

  public boolean requiresAuthorization() {
    return true;
  }

  public String getAuthorizationKey() {
    return "pwd." + ipAddress;
  }

  @Override
  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer) throws RunnerException, SerialException {
    if (usingProgrammer) {
      System.err.println("Http upload using programmer not supported");
      return false;
    }

    FilePart filePart;
    try {
      filePart = new FilePart("sketch", new File(buildPath, className + ".hex"));
    } catch (FileNotFoundException e) {
      throw new RunnerException(e);
    }

    Part[] parts = {filePart};
    PostMethod post = newPostMethod();
    post.setRequestHeader("Cookie", "pwd=" + Preferences.get(getAuthorizationKey()));
    post.setRequestEntity(new MultipartRequestEntity(parts, post.getParams()));

    int statusCode;
    try {
      statusCode = client.executeMethod(post);
    } catch (IOException e) {
      throw new RunnerException(e);
    }

    if (statusCode == HttpStatus.SC_OK) {
      return true;
    }

    try {
      System.err.println(post.getResponseBodyAsString());
      return false;
    } catch (IOException e) {
      throw new RunnerException(e);
    }
  }

  protected PostMethod newPostMethod() {
    return new PostMethod("http://" + ipAddress + ":6571/upload");
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via http");
  }

}
