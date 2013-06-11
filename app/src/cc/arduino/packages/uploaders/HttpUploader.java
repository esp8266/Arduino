package cc.arduino.packages.uploaders;

import cc.arduino.packages.Uploader;
import com.jcraft.jsch.*;
import org.apache.commons.codec.binary.Base64;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.NameValuePair;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.protocol.Protocol;
import processing.app.Base;
import processing.app.Constants;
import processing.app.NetworkMonitor;
import processing.app.Preferences;
import processing.app.debug.EasySSLProtocolSocketFactory;
import processing.app.debug.RunnerException;
import processing.app.debug.TargetPlatform;
import processing.app.helpers.PreferencesMap;

import java.io.*;
import java.util.regex.Matcher;

import static processing.app.I18n._;

public class HttpUploader extends Uploader {

  private static final String PROTOCOL = "https://";

  static {
    Protocol.registerProtocol("https", new Protocol("https", new EasySSLProtocolSocketFactory(), 443));
  }

  private final HttpClient client;
  private final String ipAddress;
  private final String baseUrl;

  public HttpUploader(String port) {
    client = new HttpClient();
    Matcher matcher = Constants.IPV4_ADDRESS.matcher(port);
    if (!matcher.find()) {
      throw new IllegalArgumentException(port);
    }
    ipAddress = matcher.group();
    baseUrl = PROTOCOL + ipAddress + "/cgi-bin/luci/arduino";
  }

  public boolean requiresAuthorization() {
    return true;
  }

  public String getAuthorizationKey() {
    return "runtime.pwd." + ipAddress;
  }

  @Override
  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer) throws RunnerException {
    if (usingProgrammer) {
      System.err.println(_("Http upload using programmer not supported"));
      return false;
    }

    client.getHttpConnectionManager().getParams().setConnectionTimeout(5000);
    String password = Preferences.get(getAuthorizationKey());
    String auth = Base64.encodeBase64String(("root:" + password).getBytes());

    int sleptTimes = 1;
    while (boardNotReady(auth)) {
      try {
        Thread.sleep(100);
      } catch (InterruptedException e) {
        throw new RunnerException(e);
      }
      if (sleptTimes >= 3) {
        throw new RunnerException(_("The board is not yet ready"));
      }
      sleptTimes += 1;
    }

    try {
      scpHexToBoard(buildPath, className);
    } catch (Exception e) {
      throw new RunnerException(e);
    }

    TargetPlatform targetPlatform = Base.getTargetPlatform();
    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));

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

  private void scpHexToBoard(String buildPath, String className) throws JSchException, IOException {
    Session session = null;
    Channel channel = null;
    OutputStream out = null;
    InputStream in = null;
    try {
      JSch jSch = new JSch();
      session = jSch.getSession("root", ipAddress, 22);
      session.setPassword(Preferences.get(getAuthorizationKey()));

      session.setUserInfo(new NetworkMonitor.NoInteractionUserInfo());
      session.connect(30000);

      channel = session.openChannel("exec");
      ((ChannelExec) channel).setCommand("scp -t /tmp/sketch.hex");

      out = channel.getOutputStream();
      in = channel.getInputStream();

      channel.connect();

      ensureAcknowledged(out, in);

      File hex = new File(buildPath, className + ".hex");

      sendFileSizeAndName(out, in, hex);
      ensureAcknowledged(out, in);

      sendFileContents(out, hex);
      ensureAcknowledged(out, in);
    } finally {
      if (out != null) {
        out.close();
      }
      if (channel != null) {
        channel.disconnect();
      }
      if (session != null) {
        session.disconnect();
      }
    }
  }

  private void ensureAcknowledged(OutputStream out, InputStream in) throws IOException {
    out.flush();

    int b = in.read();

    if (b == 0) return;
    if (b == -1) return;

    if (b == 1 || b == 2) {
      StringBuilder sb = new StringBuilder();
      sb.append("SCP error: ");

      int c;
      do {
        c = in.read();
        sb.append((char) c);
      } while (c != '\n');

      throw new IOException(sb.toString());
    }

    throw new IOException("Uknown SCP error: " + b);
  }

  private void sendFileContents(OutputStream out, File hex) throws IOException {
    // send file contents
    FileInputStream fis = null;
    try {
      fis = new FileInputStream(hex);
      byte[] buf = new byte[4096];
      while (true) {
        int len = fis.read(buf, 0, buf.length);
        if (len <= 0) break;
        out.write(buf, 0, len); //out.flush();
      }

      // send \0 (terminates file)
      buf[0] = 0;
      out.write(buf, 0, 1);
    } finally {
      if (fis != null) {
        fis.close();
      }
    }
  }

  private void sendFileSizeAndName(OutputStream out, InputStream in, File hex) throws IOException {
    // send "C0644 filesize filename"
    long filesize = hex.length();
    String command = "C0644 " + filesize + " " + hex.getName() + "\n";
    out.write(command.getBytes());
  }

  protected boolean boardNotReady(String auth) throws RunnerException {
    GetMethod get = new GetMethod(baseUrl + "/ready");
    get.setRequestHeader("Authorization", "Basic " + auth);
    try {
      int httpStatus = client.executeMethod(get);
      if (httpStatus % HttpStatus.SC_BAD_REQUEST < 100 || httpStatus % HttpStatus.SC_INTERNAL_SERVER_ERROR < 100) {
        System.err.println(get.getResponseBodyAsString());
        throw new RunnerException("Problem knowing if the board was ready");
      }
      return httpStatus != HttpStatus.SC_OK;
    } catch (IOException e) {
      return true;
    }
  }

  @Override
  public boolean burnBootloader() throws RunnerException {
    throw new RunnerException("Can't burn bootloader via http");
  }

}
