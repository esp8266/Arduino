package processing.app.helpers;

import static org.junit.Assert.assertArrayEquals;

import org.junit.Test;

public class StringReplacerTest {
  
  @Test
  public void quotingCheck() throws Exception {
    String in = "a\"bc ab'c   'abc abc' ";
    in += "\"abc abc\" '\"abc abc\"' ";
    in += "\"'abc abc'\"";
    String[] res = StringReplacer.quotedSplit(in, "\"'", false);
    assertArrayEquals(res, new String[] { "a\"bc", "ab'c", "abc abc",
        "abc abc", "\"abc abc\"", "'abc abc'" });
  }

  @Test
  public void quotingCheckWithEmptyStringsAccepted() throws Exception {
    String in = "a\"bc ab'c   'abc abc' ";
    in += "\"abc abc\" '\"abc abc\"' ";
    in += "\"'abc abc'\"";
    String[] res = StringReplacer.quotedSplit(in, "\"'", true);
    assertArrayEquals(res, new String[] { "a\"bc", "ab'c", "", "", "abc abc",
        "abc abc", "\"abc abc\"", "'abc abc'" });
  }
  
}
