package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.TokenTypes;
import org.junit.Test;
import processing.app.AbstractWithPreferencesTest;
import processing.app.BaseNoGui;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

public class PdeKeywordsTest extends AbstractWithPreferencesTest {

  @Test
  public void testKeywordsTxtParsing() throws Exception {
    PdeKeywords pdeKeywords = new PdeKeywords();
    pdeKeywords.reload();

    assertEquals("Constants", pdeKeywords.getReference("HIGH"));
    assertEquals("IDENTIFIER", pdeKeywords.getTokenTypeAsString("HIGH"));
    assertEquals(TokenTypes.IDENTIFIER, pdeKeywords.getTokenType("HIGH".toCharArray(), 0, 3));

    assertEquals("IncrementCompound", pdeKeywords.getReference("+="));
    assertNull(pdeKeywords.getTokenTypeAsString("+="));

    assertNull(pdeKeywords.getReference("Mouse"));
    assertEquals("VARIABLE", pdeKeywords.getTokenTypeAsString("Mouse"));
    assertEquals(TokenTypes.VARIABLE, pdeKeywords.getTokenType("Mouse".toCharArray(), 0, 4));
  }

}
