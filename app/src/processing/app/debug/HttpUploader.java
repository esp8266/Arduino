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

import static processing.app.I18n._;

public class HttpUploader extends Uploader {

  private final HttpClient client;
  private final String ipAddress;

  public HttpUploader() {
    this.client = new HttpClient();
    this.ipAddress = Preferences.get("serial.port").substring(0, Preferences.get("serial.port").indexOf(" "));
  }

  @Override
  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer) throws RunnerException, SerialException {
    if (usingProgrammer) {
      System.err.println("Http upload using programmer not supported");
      return false;
    }

    FilePart filePart;
    try {
      filePart = new FilePart("sketch.hex", new File(buildPath, className + ".hex"));
    } catch (FileNotFoundException e) {
      throw new RunnerException(e);
    }

    Part[] parts = {filePart};
    PostMethod post = newPostMethod();
    post.setRequestEntity(new MultipartRequestEntity(parts, post.getParams()));

    int statusCode;
    try {
      statusCode = client.executeMethod(post);
    } catch (IOException e) {
      throw new RunnerException(e);
    }

    if (statusCode == HttpStatus.SC_OK) {
      System.out.println(_("Sketch uploaded"));
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
    return new PostMethod("http://" + ipAddress + ":8000/upload");
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via http");
  }

}
