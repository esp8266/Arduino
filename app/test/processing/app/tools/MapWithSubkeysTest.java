package processing.app.tools;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.junit.Before;
import org.junit.Test;

public class MapWithSubkeysTest {

  private MapWithSubkeys map;

  @Before
  public void setup() throws Exception {
    Map<String, String> input = new HashMap<String, String>();
    BufferedReader reader = new BufferedReader(new InputStreamReader(MapWithSubkeysTest.class.getResourceAsStream("test_partial_boards.txt")));
    String line = null;
    while ((line = reader.readLine()) != null) {
      String[] lineParts = line.split("=");
      input.put(lineParts[0], lineParts[1]);
    }
    map = MapWithSubkeys.createFrom(input);
  }

  @Test
  public void shouldListCustomMenusIDs() {
    Collection<String> menusIDs = map.getKeys();

    assertEquals(2, menusIDs.size());
    assertTrue(menusIDs.contains("cpu"));
    assertTrue(menusIDs.contains("speed"));

    assertEquals("Processor", map.getValueOf("cpu"));

    MapWithSubkeys cpu = map.get("cpu");

    Collection<String> boards = cpu.getKeys();
    assertEquals(1, boards.size());
    assertTrue(boards.contains("nano"));

    Collection<String> cpuNanoProcessors = cpu.get("nano").getKeys();
    assertEquals(2, cpuNanoProcessors.size());
    assertTrue(cpuNanoProcessors.contains("atmega168"));
    assertTrue(cpuNanoProcessors.contains("atmega328"));
    assertEquals("ATmega168", cpu.get("nano").getValueOf("atmega168"));
    assertEquals("ATmega328", cpu.get("nano").getValueOf("atmega328"));

    MapWithSubkeys atmega168Properties = cpu.get("nano").get("atmega168");
    assertEquals(9, atmega168Properties.getKeys().size());
    assertTrue(atmega168Properties.getKeys().contains("bootloader.high_fuses"));
  }

}
