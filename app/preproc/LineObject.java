package processing.app.preproc;

class LineObject {
  LineObject parent = null;
  String source = "";
  int line = 1;
  boolean enteringFile = false;
  boolean returningToFile = false;
  boolean systemHeader = false;
  boolean treatAsC = false;

  public LineObject() 
  {
    super();
  }

  public LineObject( LineObject lobj )
  {
    parent = lobj.getParent();
    source = lobj.getSource();
    line = lobj.getLine();
    enteringFile = lobj.getEnteringFile();
    returningToFile = lobj.getReturningToFile();
    systemHeader = lobj.getSystemHeader();
    treatAsC = lobj.getTreatAsC();
  }

  public LineObject( String src) 
  {
    source = src;
  }

  public void setSource(String src) 
  {
    source = src;
  }

  public String getSource() 
  {
    return source;
  }

  public void setParent(LineObject par) 
  {
    parent = par;
  }

  public LineObject getParent() 
  {
    return parent;
  }

  public void setLine(int l)
  {
    line = l;
  }

  public int getLine() 
  {
    return line;
  }

  public void newline()
  {
    line++;
  }

  public void setEnteringFile(boolean v) 
  {
    enteringFile = v;
  }

  public boolean getEnteringFile() 
  {
    return enteringFile;
  }

  public void setReturningToFile(boolean v) 
  {
    returningToFile = v;
  }

  public boolean getReturningToFile() 
  {
    return returningToFile;
  }

  public void setSystemHeader(boolean v) 
  {
    systemHeader = v;
  }

  public boolean getSystemHeader() 
  {
    return systemHeader;
  }

  public void setTreatAsC(boolean v) 
  {
    treatAsC = v;
  }

  public boolean getTreatAsC() 
  {
    return treatAsC;
  }

  public String toString() {
    StringBuffer ret;
    ret = new StringBuffer("# " + line + " \"" + source + "\"");
    if (enteringFile) {
        ret.append(" 1");
    }
    if (returningToFile) {
        ret.append(" 2");
    }
    if (systemHeader) {
        ret.append(" 3");
    }
    if (treatAsC) {
        ret.append(" 4");
    }
    return ret.toString();
  }
}

