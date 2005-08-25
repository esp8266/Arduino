/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Based on Simple1.2ClassLoader.java - simple Java 1.2 class loader
  Copyright (c) 1999 Ken McCrary, All Rights Reserved.

  Permission to use, copy, modify, and distribute this software
  and its documentation for NON-COMMERCIAL purposes and without
  fee is hereby granted provided that this copyright notice
  appears in all copies.

  KEN MCCRARY MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
  SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING
  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. KEN MCCRARY
  SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT
  OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */
package processing.app;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.jar.*;


/**
 * Simple class loader adapted for Processing.
 * <P>
 * Based on code from Ken McCrary.
 */
public class RunnerClassLoader extends ClassLoader {
  String buildFolderPath;

  RunnerClassLoader() {
    buildFolderPath = Base.getBuildFolder().getAbsolutePath();
  }

  /**
   * This is the method where the task of class loading
   * is delegated to our custom loader.
   *
   * @param  name the name of the class
   * @return the resulting <code>Class</code> object
   * @exception ClassNotFoundException if the class could not be found
   */
  protected Class findClass(String name) throws ClassNotFoundException {
    FileInputStream fi = null;

    try {
      String path =
        buildFolderPath + File.separator + name.replace('.', '/');
      //System.out.println("(from " + path + ")");
      fi = new FileInputStream(path + ".class");
      byte[] classBytes = new byte[fi.available()];
      fi.read(classBytes);
      //definePackage(name);
      return defineClass(name, classBytes, 0, classBytes.length);

    } catch (Exception e) {
      // could not find the class, so indicate the problem with an exception
      throw new ClassNotFoundException(name);

    } finally {
      if (fi != null) {
        try {
          fi.close();
        } catch (Exception e) { }
      }
    }
  }


  /**
   * Identify where to load a resource from, resources for
   * this simple ClassLoader are in a directory name "store"
   *
   * @param name the resource name
   * @return URL for resource or null if not found
   */
  protected URL findResource(String name) {
    String path =
      buildFolderPath + File.separator + name.replace('.', '/');
    File searchResource = new File(path, name);
    //URL result = null;

    if (searchResource.exists()) {
      try {
        return searchResource.toURL();
      } catch (MalformedURLException mfe) { }
    }
    //return result;
    return null;
  }


  /**
   * Used for identifying resources from multiple URLS
   * Since our simple Classloader only has one repository
   * the returned Enumeration contains 0 to 1 items
   *
   * @param name the resource name
   * @return Enumeration of one URL
   */
  protected Enumeration findResources(final String name) throws IOException {
    // Since we only have a single repository we will only have one
    // resource of a particular name, the Enumeration will just return
    // this single URL

    return new Enumeration() {
        URL resource = findResource(name);

        public boolean hasMoreElements() {
          return ( resource != null ? true : false);
        }

        public Object nextElement() {
          if (!hasMoreElements()) {
            throw new NoSuchElementException();
          } else {
            URL result = resource;
            resource = null;
            return result;
          }
        }
      };
  }
}
