package processing.build;  

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
    
    File graphicsFile = new File(baseDir, "PGraphics.java"); 
    File appletFile = new File(baseDir, "PApplet.java"); 
    File imageFile = new File(baseDir, "PImage.java"); 
    
    if (!graphicsFile.exists() || !graphicsFile.canRead()) {
      throw new BuildException("PGraphics file not readable: " + 
    		  graphicsFile.getAbsolutePath()); 
    }
    
    if (!appletFile.exists() || !appletFile.canRead() || !appletFile.canWrite()) {
      throw new BuildException("PApplet file not read/writeable: " + 
    		  appletFile.getAbsolutePath()); 
    }
    
    if (!imageFile.exists() || !imageFile.canRead()) {
      throw new BuildException("PImage file not readable: " + 
    		  imageFile.getAbsolutePath()); 
    }
    
    // Looking good, let's do this!
    ByteArrayOutputStream outBytes = new ByteArrayOutputStream(); 
    PrintStream out = new PrintStream(outBytes);
    StringBuffer content = new StringBuffer();
    
    try{
      BufferedReader applet = createReader(appletFile);
      String line; 
      while ((line = applet.readLine()) != null) {
        out.println(line);
        content.append(line + "\n"); 
        
        if (line.indexOf("public functions for processing.core") >= 0) {
          break; 
        }
      }
      
      // read the rest of the file and append it to the
      while ((line = applet.readLine()) != null) {
        content.append(line + "\n"); 
      }
      
      applet.close();
      process(out, graphicsFile);
      process(out, imageFile);
      
      out.println("}");
    
    } catch (IOException e) {
      e.printStackTrace();
    
    } catch(Exception ex) {
      ex.printStackTrace(); 
    }
    out.flush(); 
    
    if (content.toString().equals(outBytes.toString())) {
      System.out.println("No changes to PApplet API.");
    } else {
      System.out.println("Updating PApplet with API changes from PImage or PGraphics.");
      try {
        PrintStream temp = new PrintStream(appletFile);  
        temp.print(outBytes.toString());
        temp.flush(); 
        temp.close(); 
      } catch (FileNotFoundException e) {
        e.printStackTrace();
      }  
    }    
  }
  
  
  private void process(PrintStream out, File input) throws IOException {
    BufferedReader in = createReader(input); 
    int comments = 0;
    String line = null;
    
    while ((line = in.readLine()) != null) {
      String decl = ""; 
      
      // Keep track of comments
      if (line.matches(Pattern.quote("/*"))) {
        comments ++; 
      }
      
      if (line.matches(Pattern.quote("*/"))) {
        comments --; 
      }
      
      // Ignore everything inside comments
      if (comments > 0) {
        continue;  
      }
      
      boolean gotSomething = false; 
      boolean gotStatic = false;
      
      Matcher result;  
      
      if ((result = Pattern.compile("^\\s*public ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true; 
      }
      else if ((result = Pattern.compile("^\\s*abstract public ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true; 
      }
      else if ((result = Pattern.compile("^\\s*public final ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
        gotSomething = true;
      }
      else if ((result = Pattern.compile("^\\s*static public ([\\w\\[\\]]+) [a-zA-z_]+\\(.*$").matcher(line)).matches()) {
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
        
        out.println("\n\n" + line);
        
        decl += line; 
        while(line.indexOf(')') == -1) {
          line = in.readLine(); 
          decl += line; 
          line = line.replaceAll("\\;\\s*$", " {\n"); 
          out.println(line); 
        }
        
        result = Pattern.compile(".*?\\s(\\S+)\\(.*?").matcher(decl);
        result.matches(); // try to match. DON't remove this or things will stop working! 
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
          out.println(rline); 
        }
        
        out.println(gline); 
        out.println("  }"); 
      }
    }
    
    in.close(); 
  }

  
  private static BufferedReader createReader(File f) throws FileNotFoundException {
    return new BufferedReader(new InputStreamReader(new FileInputStream(f))); 
  }
}
