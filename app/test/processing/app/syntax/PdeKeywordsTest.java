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
    assertEquals("RESERVED_WORD_2", pdeKeywords.getTokenTypeAsString("HIGH"));
    assertEquals(TokenTypes.RESERVED_WORD_2, pdeKeywords.getTokenType("HIGH".toCharArray(), 0, 3));

    assertEquals("IncrementCompound", pdeKeywords.getReference("+="));
    assertNull(pdeKeywords.getTokenTypeAsString("+="));

    assertNull(pdeKeywords.getReference("Mouse"));
    assertEquals("DATA_TYPE", pdeKeywords.getTokenTypeAsString("Mouse"));
    assertEquals(TokenTypes.DATA_TYPE, pdeKeywords.getTokenType("Mouse".toCharArray(), 0, 4));
  }

}
