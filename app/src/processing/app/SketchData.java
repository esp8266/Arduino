package processing.app;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class SketchData {

  /**
   * Name of sketch, which is the name of main file (without .pde or .java
   * extension)
   */
  private String name;

  private List<SketchCode> codes = new ArrayList<SketchCode>();

  private static final Comparator<SketchCode> CODE_DOCS_COMPARATOR = new Comparator<SketchCode>() {
    @Override
    public int compare(SketchCode x, SketchCode y) {
      return x.getFileName().compareTo(y.getFileName());
    }
  };

  public int getCodeCount() {
    return codes.size();
  }

  public SketchCode[] getCodes() {
    return codes.toArray(new SketchCode[0]);
  }

  public void addCode(SketchCode sketchCode) {
    codes.add(sketchCode);
  }

  public void moveCodeToFront(SketchCode codeDoc) {
    codes.remove(codeDoc);
    codes.add(0, codeDoc);
  }

  protected void replaceCode(SketchCode newCode) {
    for (SketchCode code : codes) {
      if (code.getFileName().equals(newCode.getFileName())) {
        codes.set(codes.indexOf(code), newCode);
        return;
      }
    }
  }

  protected void sortCode() {
    if (codes.size() < 2)
      return;
    SketchCode first = codes.remove(0);
    Collections.sort(codes, CODE_DOCS_COMPARATOR);
    codes.add(0, first);
  }

  public SketchCode getCode(int i) {
    return codes.get(i);
  }

  protected void removeCode(SketchCode which) {
    for (SketchCode code : codes) {
      if (code == which) {
        codes.remove(code);
        return;
      }
    }
    System.err.println("removeCode: internal error.. could not find code");
  }

  public int indexOfCode(SketchCode who) {
    for (SketchCode code : codes) {
      if (code == who)
        return codes.indexOf(code);
    }
    return -1;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public void clearCodeDocs() {
    codes.clear();
  }
}
