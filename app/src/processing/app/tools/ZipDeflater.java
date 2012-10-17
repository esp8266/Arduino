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

public class ZipDeflater {

  private final ZipFile zipFile;
  private final File destFolder;

  public ZipDeflater(File file, File destFolder) throws ZipException, IOException {
    this.destFolder = destFolder;
    this.zipFile = new ZipFile(file);
  }

  public void deflate() throws IOException {
    String folderName = tempFolderNameFromZip();

    File folder = new File(destFolder, folderName);

    if (!folder.mkdir()) {
      throw new IOException("Unable to create folder " + folderName);
    }

    Enumeration<? extends ZipEntry> entries = zipFile.entries();
    while (entries.hasMoreElements()) {
      ZipEntry entry = entries.nextElement();
      ensureFoldersOfEntryExist(folder, entry);
      File entryFile = new File(folder, entry.getName());
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

    // Test.zip may or may not contain Test folder. We use zip name to create libraries folder. Therefore, a contained Test folder is useless and must be removed
    ensureOneLevelFolder(folder);
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
    if (files.length == 1 && files[0].isDirectory()) {
      File tempFile = new File(files[0].getPath() + new Random().nextInt(1000));
      files[0].renameTo(tempFile);
      for (File file : tempFile.listFiles()) {
        file.renameTo(new File(folder, file.getName()));
      }
      tempFile.delete();
    }
  }

  private String tempFolderNameFromZip() {
    String folderName = zipFile.getName();
    if (folderName.lastIndexOf(".") != -1) {
      folderName = folderName.substring(0, folderName.lastIndexOf("."));
    }
    if (folderName.lastIndexOf(File.separator) != -1) {
      folderName = folderName.substring(folderName.lastIndexOf(File.separator) + 1);
    }
    return folderName;
  }

}
