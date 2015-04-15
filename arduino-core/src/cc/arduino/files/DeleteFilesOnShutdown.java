package cc.arduino.files;

import processing.app.helpers.FileUtils;

import java.io.File;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public class DeleteFilesOnShutdown implements Runnable {

  public static final DeleteFilesOnShutdown INSTANCE = new DeleteFilesOnShutdown();

  public static void add(File file) {
    INSTANCE.addFile(file);
  }

  private final List<File> files;

  public DeleteFilesOnShutdown() {
    this.files = new LinkedList<File>();
  }

  public synchronized void addFile(File file) {
    this.files.add(file);
  }

  @Override
  public void run() {
    List<File> copyOfFiles;
    synchronized (this) {
      copyOfFiles = new LinkedList<File>(files);
    }
    Collections.reverse(copyOfFiles);
    for (File file : copyOfFiles) {
      if (file.exists() && file.canWrite()) {
        FileUtils.recursiveDelete(file);
      }
    }
  }

}
