import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;


/**
 * Ant Task for copying the PImage and PGraphics methods into PApplet.
 */
public class PAppletMethods extends Task {

  private File baseDir;


  public PAppletMethods() {
  }


  public void setDir(String dir) {
    baseDir = new File(dir);
  }


  public void execute() throws BuildException {
    // Do a bunch of checks...
    if (baseDir == null) {
      throw new BuildException("dir parameter must be set!");
    }

    //System.out.println("using basedir " + baseDir);
    File graphicsFile = new File(baseDir, "PGraphics.java");
    File appletFile = new File(baseDir, "PApplet.java");
    File imageFile = new File(baseDir, "PImage.java");

    if (!graphicsFile.exists() || !graphicsFile.canRead()) {
      throw new BuildException("PGraphics file not readable: " +
                               graphicsFile.getAbsolutePath());
    }

    if (!appletFile.exists() ||
        !appletFile.canRead() ||
        !appletFile.canWrite()) {
      throw new BuildException("PApplet file not read/writeable: " +
                               appletFile.getAbsolutePath());
    }

    if (!imageFile.exists() || !imageFile.canRead()) {
      throw new BuildException("PImage file not readable: " +
                               imageFile.getAbsolutePath());
    }

    // Looking good, let's do this!
    //ByteArrayOutputStream outBytes = new ByteArrayOutputStream();
    //PrintStream out = new PrintStream(outBytes, "UTF-8");
    StringBuffer out = new StringBuffer();
    StringBuffer content = new StringBuffer();

    try{
      BufferedReader applet = createReader(appletFile);
      String line;
      while ((line = applet.readLine()) != null) {
        out.append(line);
        out.append('\n');  // to avoid Windows CRs
        content.append(line);
        content.append('\n');  // for efficiency

        if (line.indexOf("public functions for processing.core") >= 0) {
          break;
        }
      }

      // read the rest of the file and append it to the
      while ((line = applet.readLine()) != null) {
        content.append(line);
        content.append('\n');
      }

      applet.close();
      process(out, graphicsFile);
      process(out, imageFile);

      out.append('}');
      out.append('\n');

      //} catch (IOException e) {
      //e.printStackTrace();

    } catch (Exception e) {
      //ex.printStackTrace();
      throw new BuildException(e);
    }
    //out.flush();

    String outString = out.toString();
    if (content.toString().equals(outString)) {
      System.out.println("No changes to PApplet API.");
    } else {
      System.out.println("Updating PApplet with API changes " +
                         "from PImage or PGraphics.");
      try {
        PrintStream temp = new PrintStream(appletFile, "UTF-8");
        temp.print(outString);
        temp.flush();
        temp.close();
      } catch (IOException e) {
        //e.printStackTrace();
        throw new BuildException(e);
      }
    }
  }


  private void process(StringBuffer out, File input) throws IOException {
    BufferedReader in = createReader(input);
    int comments = 0;
    String line = null;
    StringBuffer commentBuffer = new StringBuffer();

    while ((line = in.readLine()) != null) {
      String decl = "";

      // Keep track of comments
      //if (line.matches(Pattern.quote("/*"))) {
      if (line.indexOf("/*") != -1) {
        comments++;
      }

      //if (line.matches(Pattern.quote("*/"))) {
      if (line.indexOf("*/") != -1) {
        commentBuffer.append(line);
        commentBuffer.append('\n');
        //System.out.println("comment is: " + commentBuffer.toString());
        comments--;
        // otherwise gotSomething will be false, and nuke the comment
        continue;
      }

      // Ignore everything inside comments
      if (comments > 0) {
        commentBuffer.append(line);
        commentBuffer.append('\n');
        continue;
      }

      boolean gotSomething = false;
      boolean gotStatic = false;

      Matcher result;

      if ((result = Pattern.compile("^\\s*public ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true;

      } else if ((result = Pattern.compile("^\\s*abstract public ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true;

      } else if ((result = Pattern.compile("^\\s*public final ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true;

      } else if ((result = Pattern.compile("^\\s*static public ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true;
        gotStatic = true;
      }

      // if function is marked "// ignore" then, uh, ignore it.
      if (gotSomething && line.indexOf("// ignore") >= 0) {
        gotSomething = false;
      }

      String returns = "";
      if (gotSomething) {
        if (result.group(1).equals("void")) {
          returns = "";
        } else {
          returns = "return ";
        }

        // remove the abstract modifier
        line = line.replaceFirst(Pattern.quote("abstract"), " ");

        // replace semicolons with a start def
        line = line.replaceAll(Pattern.quote(";"), " {\n");

        //out.println("\n\n" + line);
        out.append('\n');
        out.append('\n');
        // end has its own newline
        //out.print(commentBuffer.toString());  // TODO disabled for now XXXX
        out.append(commentBuffer.toString());  // duplicates all comments
        commentBuffer.setLength(0);
        out.append(line);
        out.append('\n');

        decl += line;
        while(line.indexOf(')') == -1) {
          line = in.readLine();
          decl += line;
          line = line.replaceAll("\\;\\s*$", " {\n");
          out.append(line);
          out.append('\n');
        }

        result = Pattern.compile(".*?\\s(\\S+)\\(.*?").matcher(decl);
        // try to match. don't remove this or things will stop working!
        result.matches();
        String declName = result.group(1);
        String gline = "";
        String rline = "";
        if (gotStatic) {
          gline = "    " + returns + "PGraphics." + declName + "(";
        } else {
          rline = "    if (recorder != null) recorder." + declName + "(";
          gline = "    " + returns + "g." + declName + "(";
        }

        decl = decl.replaceAll("\\s+", " "); // smush onto a single line
        decl = decl.replaceFirst("^.*\\(", "");
        decl = decl.replaceFirst("\\).*$", "");

        int prev = 0;
        String parts[] = decl.split("\\, ");

        for (String part : parts) {
          if (!part.trim().equals("")) {
            String blargh[] = part.split(" ");
            String theArg = blargh[1].replaceAll("[\\[\\]]", "");

            if (prev != 0) {
              gline += ", ";
              rline += ", ";
            }

            gline += theArg;
            rline += theArg;
            prev = 1;
          }
        }

        gline += ");";
        rline += ");";

        if (!gotStatic && returns.equals("")) {
          out.append(rline);
          out.append('\n');
        }
        out.append(gline);
        out.append('\n');
        out.append("  }");
        out.append('\n');

      } else {
        commentBuffer.setLength(0);
      }
    }

    in.close();
  }


  static BufferedReader createReader(File file) throws IOException {
    FileInputStream fis = new FileInputStream(file);
    return new BufferedReader(new InputStreamReader(fis, "UTF-8"));
  }
}
