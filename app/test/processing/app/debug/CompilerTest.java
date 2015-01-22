package processing.app.debug;

import static org.junit.Assert.assertEquals;
import static processing.app.debug.Compiler.unescapeDepFile;

import org.junit.Test;

import processing.app.AbstractWithPreferencesTest;

public class CompilerTest extends AbstractWithPreferencesTest {

  @Test
  public void makeDepUnescapeTest() throws Exception {
    assertEquals("C:\\Arduino\\hardware\\arduino\\avr\\cores\\arduino\\Stream.cpp",
                 unescapeDepFile("C:\\Arduino\\hardware\\arduino\\avr\\cores\\arduino\\Stream.cpp"));
    assertEquals("C:\\Arduino 1.5.3\\hardware\\arduino\\avr\\cores\\arduino\\Stream.cpp",
                 unescapeDepFile("C:\\Arduino\\ 1.5.3\\hardware\\arduino\\avr\\cores\\arduino\\Stream.cpp"));
    assertEquals("C:\\Ard$ui#\\\\ no 1.5.3\\hardware\\arduino\\avr\\cores\\arduino\\Stream.cpp",
                 unescapeDepFile("C:\\Ard$$ui\\#\\\\\\\\\\ no 1.5.3\\hardware\\arduino\\avr\\cores\\arduino\\Stream.cpp"));
  }

}
