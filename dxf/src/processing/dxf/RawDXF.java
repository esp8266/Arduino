/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 * RawDXF - Code to write DXF files with beginRaw/endRaw
 * An extension for the Processing project - http://processing.org
 * <p/>
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * <p/>
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * <p/>
 * You should have received a copy of the GNU Lesser General
 * Public License along with the Processing project; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 */

package processing.dxf;

import java.io.*;

import processing.core.*;


/**
 * A simple library to write DXF files with Processing.
 * Because this is used with beginRaw() and endRaw(), only individual 
 * triangles and (discontinuous) line segments will be written to the file. 
 * <P/>
 * Use something like a keyPressed() in PApplet to trigger it,
 * to avoid writing a bazillion .dxf files.
 * <P/>
 * Usually, the file will be saved to the sketch's folder.
 * Use Sketch &rarr; Show Sketch Folder to see it from the PDE.
 * <p/>
 * A simple example of how to use:
 * <PRE>
 * import processing.dxf.*;
 *
 * boolean record;
 *
 * void setup() {
 *   size(500, 500, P3D);
 * }
 *
 * void keyPressed() {
 *   // use a key press so that it doesn't make a million files
 *   if (key == 'r') record = true;
 * }
 *
 * void draw() {
 *   if (record) {
 *     beginRaw(DXF, "output.dxf");
 *   }
 *
 *   // do all your drawing here
 *
 *   if (record) {
 *     endRaw();
 *     record = false;
 *   }
 * }
 * </PRE>
 * or to use it and be able to control the current layer:
 * <PRE>
 * import processing.dxf.*;
 *
 * boolean record;
 * RawDXF dxf;
 *
 * void setup() {
 *   size(500, 500, P3D);
 * }
 *
 * void keyPressed() {
 *   // use a key press so that it doesn't make a million files
 *   if (key == 'r') record = true;
 * }
 *
 * void draw() {
 *   if (record) {
 *     dxf = (RawDXF) createGraphics(width, height, DXF, "output.dxf");
 *     beginRaw(dxf);
 *   }
 *
 *   // do all your drawing here, and to set the layer, call:
 *   // if (record) {
 *   //   dxf.setLayer(num);
 *   // }
 *   // where 'num' is an integer.
 *   // the default is zero, or you can set it to whatever.
 *
 *   if (record) {
 *     endRaw();
 *     record = false;
 *     dxf = null;
 *   }
 * }
 * </PRE>
 * Note that even though this class is a subclass of PGraphics, it only
 * implements the parts of the API that are necessary for beginRaw/endRaw.
 * <P/>
 * Based on the original DXF writer from Simon Greenwold, February 2004.
 * Updated for Processing 0070 by Ben Fry in September 2004,
 * and again for Processing beta in April 2005.
 * Rewritten to support beginRaw/endRaw by Ben Fry in February 2006.
 * Updated again for inclusion as a core library in March 2006.
 * Constructor modifications in September 2008 as we approach 1.0.
 */
public class RawDXF extends PGraphics3D {

  File file;
  PrintWriter writer;
  int currentLayer;


  public RawDXF() { }
  
  
  public void setPath(String path) {
    this.path = path;
    if (path != null) {
      file = new File(path);
      if (!file.isAbsolute()) file = null;
    }
    if (file == null) {
      throw new RuntimeException("PGraphicsPDF requires an absolute path " +
                                 "for the location of the output file.");
    }
  }


  // ..............................................................


  protected void allocate() {
    /*
    for (int i = 0; i < MAX_TRI_LAYERS; i++) {
      layerList[i] = NO_LAYER;
    }
    */
    setLayer(0);
  }


  public void dispose() {
    writeFooter();

    writer.flush();
    writer.close();
    writer = null;
  }


  public boolean displayable() {
    return false;  // just in case someone wants to use this on its own
  }


  // ..............................................................


  public void beginDraw() {
    // have to create file object here, because the name isn't yet
    // available in allocate()
    if (writer == null) {
      try {
        writer = new PrintWriter(new FileWriter(file));
      } catch (IOException e) {
        throw new RuntimeException(e);  // java 1.4+
      }
      writeHeader();
    }
  }


  public void endDraw() {
    // nothing done here
  }


  // ..............................................................


  /**
   * Set the current layer being used in the DXF file.
   * The default is zero.
   */
  public void setLayer(int layer) {
    currentLayer = layer;
  }


  // ..............................................................


  private void writeHeader() {
    writer.println("0");
    writer.println("SECTION");
    writer.println("2");
    writer.println("ENTITIES");
  }


  private void writeFooter() {
    writer.println("0");
    writer.println("ENDSEC");
    writer.println("0");
    writer.println("EOF");
  }


  /**
   * Write a command on one line (as a String), then start a new line
   * and write out a formatted float. Available for anyone who wants to
   * insert additional commands into the DXF stream.
   */
  public void write(String cmd, float val) {
    writer.println(cmd);
    // don't format, will cause trouble on systems that aren't en-us
    // http://dev.processing.org/bugs/show_bug.cgi?id=495
    writer.println(val);
  }


  /**
   * Write a line to the dxf file. Available for anyone who wants to
   * insert additional commands into the DXF stream.
   */
  public void println(String what) {
    writer.println(what);
  }


  protected void writeLine(int index1, int index2) {
    writer.println("0");
    writer.println("LINE");

    // write out the layer
    writer.println("8");
    writer.println(String.valueOf(currentLayer));

    write("10", vertices[index1][X]);
    write("20", vertices[index1][Y]);
    write("30", vertices[index1][Z]);

    write("11", vertices[index2][X]);
    write("21", vertices[index2][Y]);
    write("31", vertices[index2][Z]);
  }


  /*
  protected void writeLineStrip() {
    writeLine();
    // shift the last vertex to be the first vertex
    System.arraycopy(vertices[1], 0, vertices[0], 0, vertices[1].length);
    vertexCount = 1;
  }
  */


  protected void writeTriangle() {
    writer.println("0");
    writer.println("3DFACE");

    // write out the layer
    writer.println("8");
    /*
    if (i < MAX_TRI_LAYERS) {
      if (layerList[i] >= 0) {
        currentLayer = layerList[i];
      }
    }
    */
    writer.println(String.valueOf(currentLayer));

    write("10", vertices[0][X]);
    write("20", vertices[0][Y]);
    write("30", vertices[0][Z]);

    write("11", vertices[1][X]);
    write("21", vertices[1][Y]);
    write("31", vertices[1][Z]);

    write("12", vertices[2][X]);
    write("22", vertices[2][Y]);
    write("32", vertices[2][Z]);

    // without adding EPSILON, rhino kinda freaks out
    // a face is actually a quad, not a triangle,
    // so instead kinda fudging the final point here.
    write("13", vertices[2][X] + EPSILON);
    write("23", vertices[2][Y] + EPSILON);
    write("33", vertices[2][Z] + EPSILON);

    vertexCount = 0;
  }


  // ..............................................................


  public void beginShape(int kind) {
    shape = kind;

    if ((shape != LINES) &&
        (shape != TRIANGLES) &&
        (shape != POLYGON)) {
      String err =
        "RawDXF can only be used with beginRaw(), " +
        "because it only supports lines and triangles";
      throw new RuntimeException(err);
    }

    if ((shape == POLYGON) && fill) {
      throw new RuntimeException("RawDXF only supports non-filled shapes.");
    }

    vertexCount = 0;
  }


  public void vertex(float x, float y) {
    vertex(x, y, 0);
  }


  public void vertex(float x, float y, float z) {
    float vertex[] = vertices[vertexCount];

    vertex[X] = x;  // note: not mx, my, mz like PGraphics3
    vertex[Y] = y;
    vertex[Z] = z;

    if (fill) {
      vertex[R] = fillR;
      vertex[G] = fillG;
      vertex[B] = fillB;
      vertex[A] = fillA;
    }

    if (stroke) {
      vertex[SR] = strokeR;
      vertex[SG] = strokeG;
      vertex[SB] = strokeB;
      vertex[SA] = strokeA;
      vertex[SW] = strokeWeight;
    }

    if (textureImage != null) {  // for the future?
      vertex[U] = textureU;
      vertex[V] = textureV;
    }
    vertexCount++;

    if ((shape == LINES) && (vertexCount == 2)) {
      writeLine(0, 1);
      vertexCount = 0;

/*
    } else if ((shape == LINE_STRIP) && (vertexCount == 2)) {
      writeLineStrip();
*/

    } else if ((shape == TRIANGLES) && (vertexCount == 3)) {
      writeTriangle();
    }
  }


  public void endShape(int mode) {
    if (shape == POLYGON) {
      for (int i = 0; i < vertexCount - 1; i++) {
        writeLine(i, i+1);
      }
      if (mode == CLOSE) {
        writeLine(vertexCount - 1, 0);
      }
    }
    /*
    if ((vertexCount != 0) &&
        ((shape != LINE_STRIP) && (vertexCount != 1))) {
      System.err.println("Extra vertex boogers found.");
    }
    */
  }
}
