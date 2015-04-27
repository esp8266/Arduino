package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.AbstractTokenMakerFactory;
import org.fife.ui.rsyntaxtextarea.RSyntaxDocument;
import org.fife.ui.rsyntaxtextarea.TokenMaker;

public class ArduinoTokenMakerFactory extends AbstractTokenMakerFactory {

  private final PdeKeywords pdeKeywords;

  public ArduinoTokenMakerFactory(PdeKeywords pdeKeywords) {
    this.pdeKeywords = pdeKeywords;
  }

  @Override
  protected TokenMaker getTokenMakerImpl(String key) {
    return new SketchTokenMaker(pdeKeywords);
  }

  @Override
  protected void initTokenMakerMap() {
    putMapping(RSyntaxDocument.SYNTAX_STYLE_CPLUSPLUS, SketchTokenMaker.class.getName());
  }

}
