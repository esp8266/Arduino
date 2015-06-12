/*
 * This file is part of Arduino.
 *
 * Copyright 2014 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package cc.arduino.utils;

import org.apache.commons.compress.archivers.ArchiveEntry;
import org.apache.commons.compress.archivers.ArchiveInputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.archivers.zip.ZipArchiveInputStream;
import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;
import org.apache.commons.compress.compressors.gzip.GzipCompressorInputStream;
import org.apache.commons.compress.utils.IOUtils;
import processing.app.I18n;
import processing.app.Platform;

import java.io.*;
import java.util.HashMap;
import java.util.Map;

import static processing.app.I18n._;

public class ArchiveExtractor {

  private final Platform platform;

  public ArchiveExtractor(Platform platform) {
    assert platform != null;
    this.platform = platform;
  }

  /**
   * Extract <b>source</b> into <b>destFolder</b>. <b>source</b> file archive
   * format is autodetected from file extension.
   *
   * @param archiveFile
   * @param destFolder
   * @throws IOException
   */
  public void extract(File archiveFile, File destFolder) throws IOException, InterruptedException {
    extract(archiveFile, destFolder, 0);
  }

  /**
   * Extract <b>source</b> into <b>destFolder</b>. <b>source</b> file archive
   * format is autodetected from file extension.
   *
   * @param archiveFile Archive file to extract
   * @param destFolder  Destination folder
   * @param stripPath   Number of path elements to strip from the paths contained in the
   *                    archived files
   * @throws IOException
   */
  public void extract(File archiveFile, File destFolder, int stripPath) throws IOException, InterruptedException {
    extract(archiveFile, destFolder, stripPath, false);
  }


  public void extract(File archiveFile, File destFolder, int stripPath, boolean overwrite) throws IOException, InterruptedException {

    // Folders timestamps must be set at the end of archive extraction
    // (because creating a file in a folder alters the folder's timestamp)
    Map<File, Long> foldersTimestamps = new HashMap<File, Long>();

    ArchiveInputStream in = null;
    try {

      // Create an ArchiveInputStream with the correct archiving algorithm
      if (archiveFile.getName().endsWith("tar.bz2")) {
        in = new TarArchiveInputStream(new BZip2CompressorInputStream(new FileInputStream(archiveFile)));
      } else if (archiveFile.getName().endsWith("zip")) {
        in = new ZipArchiveInputStream(new FileInputStream(archiveFile));
      } else if (archiveFile.getName().endsWith("tar.gz")) {
        in = new TarArchiveInputStream(new GzipCompressorInputStream(new FileInputStream(archiveFile)));
      } else if (archiveFile.getName().endsWith("tar")) {
        in = new TarArchiveInputStream(new FileInputStream(archiveFile));
      } else {
        throw new IOException("Archive format not supported.");
      }

      String pathPrefix = "";

      Map<File, File> hardLinks = new HashMap<File, File>();
      Map<File, Integer> hardLinksMode = new HashMap<File, Integer>();
      Map<File, String> symLinks = new HashMap<File, String>();
      Map<File, Long> symLinksModifiedTimes = new HashMap<File, Long>();

      // Cycle through all the archive entries
      while (true) {
        ArchiveEntry entry = in.getNextEntry();
        if (entry == null) {
          break;
        }

        // Extract entry info
        long size = entry.getSize();
        String name = entry.getName();
        boolean isDirectory = entry.isDirectory();
        boolean isLink = false;
        boolean isSymLink = false;
        String linkName = null;
        Integer mode = null;
        long modifiedTime = entry.getLastModifiedDate().getTime();

        {
          // Skip MacOSX metadata
          // http://superuser.com/questions/61185/why-do-i-get-files-like-foo-in-my-tarball-on-os-x
          int slash = name.lastIndexOf('/');
          if (slash == -1) {
            if (name.startsWith("._")) {
              continue;
            }
          } else {
            if (name.substring(slash + 1).startsWith("._")) {
              continue;
            }
          }
        }

        // Skip git metadata
        // http://www.unix.com/unix-for-dummies-questions-and-answers/124958-file-pax_global_header-means-what.html
        if (name.contains("pax_global_header")) {
          continue;
        }

        if (entry instanceof TarArchiveEntry) {
          TarArchiveEntry tarEntry = (TarArchiveEntry) entry;
          mode = tarEntry.getMode();
          isLink = tarEntry.isLink();
          isSymLink = tarEntry.isSymbolicLink();
          linkName = tarEntry.getLinkName();
        }

        // On the first archive entry, if requested, detect the common path
        // prefix to be stripped from filenames
        if (stripPath > 0 && pathPrefix.isEmpty()) {
          int slash = 0;
          while (stripPath > 0) {
            slash = name.indexOf("/", slash);
            if (slash == -1) {
              throw new IOException("Invalid archive: it must contains a single root folder");
            }
            slash++;
            stripPath--;
          }
          pathPrefix = name.substring(0, slash);
        }

        // Strip the common path prefix when requested
        if (!name.startsWith(pathPrefix)) {
          throw new IOException("Invalid archive: it must contains a single root folder while file " + name + " is outside " + pathPrefix);
        }
        name = name.substring(pathPrefix.length());
        if (name.isEmpty()) {
          continue;
        }
        File outputFile = new File(destFolder, name);

        File outputLinkedFile = null;
        if (isLink) {
          if (!linkName.startsWith(pathPrefix)) {
            throw new IOException("Invalid archive: it must contains a single root folder while file " + linkName + " is outside " + pathPrefix);
          }
          linkName = linkName.substring(pathPrefix.length());
          outputLinkedFile = new File(destFolder, linkName);
        }
        if (isSymLink) {
          // Symbolic links are referenced with relative paths
          outputLinkedFile = new File(linkName);
          if (outputLinkedFile.isAbsolute()) {
            System.err.println(I18n.format(_("Warning: file {0} links to an absolute path {1}"), outputFile, outputLinkedFile));
            System.err.println();
          }
        }

        // Safety check
        if (isDirectory) {
          if (outputFile.isFile() && !overwrite) {
            throw new IOException("Can't create folder " + outputFile + ", a file with the same name exists!");
          }
        } else {
          // - isLink
          // - isSymLink
          // - anything else
          if (outputFile.exists() && !overwrite) {
            throw new IOException("Can't extract file " + outputFile + ", file already exists!");
          }
        }

        // Extract the entry
        if (isDirectory) {
          if (!outputFile.exists() && !outputFile.mkdirs()) {
            throw new IOException("Could not create folder: " + outputFile);
          }
          foldersTimestamps.put(outputFile, modifiedTime);
        } else if (isLink) {
          hardLinks.put(outputFile, outputLinkedFile);
          hardLinksMode.put(outputFile, mode);
        } else if (isSymLink) {
          symLinks.put(outputFile, linkName);
          symLinksModifiedTimes.put(outputFile, modifiedTime);
        } else {
          // Create the containing folder if not exists
          if (!outputFile.getParentFile().isDirectory()) {
            outputFile.getParentFile().mkdirs();
          }
          copyStreamToFile(in, size, outputFile);
          outputFile.setLastModified(modifiedTime);
        }

        // Set file/folder permission
        if (mode != null && !isSymLink && outputFile.exists()) {
          platform.chmod(outputFile, mode);
        }
      }

      for (Map.Entry<File, File> entry : hardLinks.entrySet()) {
        if (entry.getKey().exists() && overwrite) {
          entry.getKey().delete();
        }
        platform.link(entry.getValue(), entry.getKey());
        Integer mode = hardLinksMode.get(entry.getKey());
        if (mode != null) {
          platform.chmod(entry.getKey(), mode);
        }
      }

      for (Map.Entry<File, String> entry : symLinks.entrySet()) {
        if (entry.getKey().exists() && overwrite) {
          entry.getKey().delete();
        }
        platform.symlink(entry.getValue(), entry.getKey());
        entry.getKey().setLastModified(symLinksModifiedTimes.get(entry.getKey()));
      }

    } finally {
      IOUtils.closeQuietly(in);
    }

    // Set folders timestamps
    for (File folder : foldersTimestamps.keySet()) {
      folder.setLastModified(foldersTimestamps.get(folder));
    }
  }

  private static void copyStreamToFile(InputStream in, long size, File outputFile) throws IOException {
    FileOutputStream fos = null;
    try {
      fos = new FileOutputStream(outputFile);
      // if size is not available, copy until EOF...
      if (size == -1) {
        byte buffer[] = new byte[4096];
        int length;
        while ((length = in.read(buffer)) != -1) {
          fos.write(buffer, 0, length);
        }
        return;
      }

      // ...else copy just the needed amount of bytes
      byte buffer[] = new byte[4096];
      while (size > 0) {
        int length = in.read(buffer);
        if (length <= 0) {
          throw new IOException("Error while extracting file " + outputFile.getAbsolutePath());
        }
        fos.write(buffer, 0, length);
        size -= length;
      }
    } finally {
      IOUtils.closeQuietly(fos);
    }
  }

}
