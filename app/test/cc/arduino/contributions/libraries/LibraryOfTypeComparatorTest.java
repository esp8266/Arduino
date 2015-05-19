package cc.arduino.contributions.libraries;

import org.junit.Test;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import static org.junit.Assert.assertEquals;

public class LibraryOfTypeComparatorTest {

  @Test
  public void testSort() throws Exception {
    List<String> strings = Arrays.asList("Arduino", "Contributed", "Recommended", "Recommended", "Other", "1yetanother", "Arduino", "Recommended", "Contributed", "Recommended");
    Collections.sort(strings, new LibraryTypeComparator());
    assertEquals(Arrays.asList("Arduino", "Arduino", "Recommended", "Recommended", "Recommended", "Recommended", "Contributed", "Contributed", "1yetanother", "Other"), strings);
  }
}
