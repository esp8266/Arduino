import java.io.Reader;
import java.io.File;
import java.io.FileReader;

import java.util.List;
import java.util.ArrayList;

import com.oroinc.text.regex.*;

public class test_PdePreprocessor {
  /************************************************************************
   Paste from PdePreprocessor.java: strip(), collapseBraces(), prototypes()
   ************************************************************************/
  
  public static void main(String[] args) {
    if (args.length < 2) {
      System.err.println("Usage: PreProc [strip|prototypes] <file>");
      return;
    }
    
    try {
      test_PdePreprocessor preproc = new test_PdePreprocessor();
      Reader reader = new FileReader(new File(args[1]));
      StringBuffer buffer = new StringBuffer();
      char[] buf = new char[1024];
      int n;

      while ((n = reader.read(buf, 0, 1024)) != -1) {
        buffer.append(buf, 0, n);
      }
      
      if (args[0].equals("strip")) {
        System.out.print(preproc.strip(buffer.toString()));
      } else {
        List prototypes = preproc.prototypes(buffer.toString());
        for (int i = 0; i < prototypes.size(); i++) {
          System.out.println((String) prototypes.get(i));
        }
      }
    } catch (Exception e) {
      System.err.println(e);
    }
  }
}