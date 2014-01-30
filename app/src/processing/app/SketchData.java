package processing.app;

import static processing.app.I18n._;

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
  
  private List<SketchCodeDocument> codeDocs = new ArrayList<SketchCodeDocument>();

  private static final Comparator<SketchCodeDocument> CODE_DOCS_COMPARATOR = new Comparator<SketchCodeDocument>() {
    @Override
    public int compare(SketchCodeDocument cd1, SketchCodeDocument cd2) {
      return cd1.getCode().getFileName().compareTo(cd2.getCode().getFileName());
    }
  };


  public int getCodeCount() {
    return codeDocs.size();
  }

  public SketchCodeDocument[] getCodeDocs() {
    return codeDocs.toArray(new SketchCodeDocument[0]);
  }

  public void addCodeDoc(SketchCodeDocument sketchCodeDoc) {
    codeDocs.add(sketchCodeDoc);
  }

  public void moveCodeDocToFront(SketchCodeDocument codeDoc) {
    codeDocs.remove(codeDoc);
    codeDocs.add(0, codeDoc);
  }

  protected void replaceCode(SketchCode newCode) {
    for (SketchCodeDocument codeDoc : codeDocs) {
      if (codeDoc.getCode().getFileName().equals(newCode.getFileName())) {
        codeDoc.setCode(newCode);
        return;
      }
    }
  }

  protected void insertCode(SketchCode sketchCode) {
    addCodeDoc(new SketchCodeDocument(sketchCode, null));
  }

  protected void sortCode() {
    if (codeDocs.size() < 2)
      return;
    SketchCodeDocument first = codeDocs.remove(0);
    Collections.sort(codeDocs, CODE_DOCS_COMPARATOR);
    codeDocs.add(0, first);
  }

  public SketchCodeDocument getCodeDoc(int i) {
    return codeDocs.get(i);
  }

  public SketchCode getCode(int i) {
    return codeDocs.get(i).getCode();
  }

  protected void removeCode(SketchCode which) {
    for (SketchCodeDocument codeDoc : codeDocs) {
      if (codeDoc.getCode() == which) {
        codeDocs.remove(codeDoc);
        return;
      }
    }
    System.err.println(_("removeCode: internal error.. could not find code"));
  }

  public int indexOfCodeDoc(SketchCodeDocument codeDoc) {
    return codeDocs.indexOf(codeDoc);
  }

  public int indexOfCode(SketchCode who) {
    for (SketchCodeDocument codeDoc : codeDocs) {
      if (codeDoc.getCode() == who) {
        return codeDocs.indexOf(codeDoc);
      }
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
    codeDocs.clear();
  }
}
