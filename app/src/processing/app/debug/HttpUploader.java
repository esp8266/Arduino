package processing.app.debug;

import org.apache.commons.codec.binary.Base64;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.NameValuePair;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import processing.app.Base;
import processing.app.Preferences;
import processing.app.SerialException;
import processing.app.helpers.PreferencesMap;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HttpUploader extends Uploader {

  private static final Pattern IPV4_ADDRESS = Pattern.compile("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");
  private static final String PROTOCOL = "http://";

  /*
  static {
    Protocol.registerProtocol("https", new Protocol("https", new EasySSLProtocolSocketFactory(), 443));
  }
  */

  private final HttpClient client;
  private final String ipAddress;
  private final String baseUrl;

  public HttpUploader(String port) {
    this.client = new HttpClient();
    Matcher matcher = IPV4_ADDRESS.matcher(port);
    if (!matcher.find()) {
      throw new IllegalArgumentException(port);
    }
    this.ipAddress = matcher.group(1);
    this.baseUrl = PROTOCOL + ipAddress + "/cgi-bin/luci/arduino";
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

    this.client.getHttpConnectionManager().getParams().setConnectionTimeout(5000);
    String auth = Base64.encodeBase64String(("root:" + Preferences.get(getAuthorizationKey())).getBytes());

    int sleptTimes = 1;
    while (boardNotReady(auth)) {
      try {
        Thread.sleep(100);
      } catch (InterruptedException e) {
        throw new RunnerException(e);
      }
      if (sleptTimes >= 3) {
        throw new RunnerException("The board is not yet ready");
      }
      sleptTimes += 1;
    }

    StringBuilder uploadRequest = new StringBuilder();

    uploadRequest.append(String.format("PWD%1$04d", Preferences.get(getAuthorizationKey()).length())).append(Preferences.get(getAuthorizationKey())).append("\n");
    uploadRequest.append("SKETCH\n");
    readSketchFile(buildPath, className, uploadRequest);
    uploadRequest.append("SKETCH_END\n");
    uploadRequest.append("EOF\n");

    Socket socket = null;
    try {
      socket = new Socket();
      socket.connect(new InetSocketAddress(ipAddress, 9876), 5000);
      OutputStreamWriter osw = new OutputStreamWriter(socket.getOutputStream());
      osw.write(uploadRequest.toString());
      osw.flush();
      BufferedReader isr = new BufferedReader(new InputStreamReader(socket.getInputStream()));
      String ok = isr.readLine();
      if (!"OK".equals(ok)) {
        throw new RunnerException("Problem uploading sketch");
      }
    } catch (IOException e) {
      throw new RunnerException(e);
    } finally {
      if (socket != null) {
        try {
          socket.close();
        } catch (IOException e) {
          // ignore
        }
      }
    }

    TargetPlatform targetPlatform = Base.getTargetPlatform();
    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));
    boolean verbose = prefs.containsKey("upload.verbose") && Boolean.parseBoolean(prefs.get("upload.verbose"));

    PostMethod post = new PostMethod(baseUrl + "/flash");
    post.setRequestHeader("Authorization", "Basic " + auth);
    NameValuePair[] data = {
            new NameValuePair("params", verbose ? prefs.get("upload.params.verbose") : prefs.get("upload.params.quiet"))
    };
    post.setRequestBody(data);

    int statusCode;
    try {
      statusCode = client.executeMethod(post);

      System.err.println(post.getResponseBodyAsString());
      return statusCode == HttpStatus.SC_OK;
    } catch (IOException e) {
      throw new RunnerException(e);
    } finally {
      post.releaseConnection();
    }
  }

  private void readSketchFile(String buildPath, String className, StringBuilder uploadRequest) throws RunnerException {
    BufferedReader sketchReader = null;
    try {
      sketchReader = new BufferedReader(new InputStreamReader(new FileInputStream(new File(buildPath, className + ".hex"))));
      String line;
      while ((line = sketchReader.readLine()) != null) {
        line = line.trim();
        if (line.length() > 0) {
          uploadRequest.append(line).append("\n");
        }
      }
    } catch (IOException e) {
      throw new RunnerException(e);
    } finally {
      if (sketchReader != null) {
        try {
          sketchReader.close();
        } catch (IOException e) {
          // ignore
        }
      }
    }
  }

  protected boolean boardNotReady(String auth) {
    GetMethod get = new GetMethod(baseUrl + "/ready");
    get.setRequestHeader("Authorization", "Basic " + auth);
    try {
      int httpStatus = client.executeMethod(get);
      return httpStatus != HttpStatus.SC_OK;
    } catch (IOException e) {
      e.printStackTrace();
      return true;
    }
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via http");
  }

}
