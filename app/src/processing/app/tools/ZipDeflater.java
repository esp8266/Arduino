package processing.app.tools;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Random;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;

import processing.app.helpers.FileUtils;

public class ZipDeflater {

  private final ZipFile zipFile;
  private final File destFolder;
  private final Random random;
  private final File file;

  public ZipDeflater(File file, File destFolder) throws ZipException, IOException {
    this.file = file;
    this.destFolder = destFolder;
    this.zipFile = new ZipFile(file);
    this.random = new Random();
  }

  public void deflate() throws IOException {
    String tmpFolderName = folderNameFromZip() + random.nextInt(1000000);

    File tmpFolder = new File(destFolder, tmpFolderName);

    if (!tmpFolder.mkdir()) {
      throw new IOException("Unable to create folder " + tmpFolderName);
    }

    Enumeration<? extends ZipEntry> entries = zipFile.entries();
    while (entries.hasMoreElements()) {
      ZipEntry entry = entries.nextElement();
      ensureFoldersOfEntryExist(tmpFolder, entry);
      File entryFile = new File(tmpFolder, entry.getName());
      if (entry.isDirectory()) {
        entryFile.mkdir();
      } else {
        FileOutputStream fos = null;
        InputStream zipInputStream = null;
        try {
          fos = new FileOutputStream(entryFile);
          zipInputStream = zipFile.getInputStream(entry);
          byte[] buffer = new byte[1024 * 4];
          int len = -1;
          while ((len = zipInputStream.read(buffer)) != -1) {
            fos.write(buffer, 0, len);
          }
        } finally {
          if (fos != null) {
            fos.close();
          }
          if (zipInputStream != null) {
            zipInputStream.close();
          }
        }
      }
    }

    deleteUndesiredFoldersAndFiles(tmpFolder);

    // Test.zip may or may not contain Test folder. If it does, we keep it. If not, we use zip name.
    ensureOneLevelFolder(tmpFolder);
  }

  private void deleteUndesiredFoldersAndFiles(File folder) {
    for (File file : folder.listFiles()) {
      if (file.isDirectory() && "__MACOSX".equals(file.getName())) {
        FileUtils.recursiveDelete(file);
      } else if (file.getName().startsWith(".")) {
        FileUtils.recursiveDelete(file);
      }
    }
  }

  private void ensureFoldersOfEntryExist(File folder, ZipEntry entry) {
    String[] parts = entry.getName().split("/");
    File current = folder;
    for (int i = 0; i < parts.length - 1; i++) {
      current = new File(current, parts[i]);
      current.mkdir();
    }
  }

  private void ensureOneLevelFolder(File folder) {
    File[] files = folder.listFiles();

    if (files.length != 1) {
      folder.renameTo(new File(folder.getParentFile(), folderNameFromZip()));
      return;
    }

    files[0].renameTo(new File(folder.getParentFile(), files[0].getName()));
    FileUtils.recursiveDelete(folder);
  }

  private String folderNameFromZip() {
    String filename = file.getName();
    if (filename.lastIndexOf(".") != -1) {
      filename = filename.substring(0, filename.lastIndexOf("."));
    }
    return filename;
  }

}
