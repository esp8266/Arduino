package processing.app.tools;

import java.util.*;
import java.util.Map.Entry;

public class MapWithSubkeys {

  public static MapWithSubkeys createFrom(Map<String, String> input) {
    if (input == null) {
      return null;
    }
    MapWithSubkeys target = new MapWithSubkeys();
    for (Entry<String, String> entry : input.entrySet()) {
      String[] entryParts = entry.getKey().split("\\.");
      if (entryParts.length == 1) {
        target.put(entryParts[0], entry.getValue());
      } else if (entryParts.length == 3) {
        target.get(entryParts[0]).get(entryParts[1]).put(entryParts[2], entry.getValue());
      } else if (entryParts.length > 3) {
        StringBuilder sb = new StringBuilder();
        for (int i = 3; i < entryParts.length; i++) {
          sb.append(entryParts[i]).append(".");
        }
        sb.deleteCharAt(sb.length() - 1);
        String key = sb.toString();
        target.get(entryParts[0]).get(entryParts[1]).get(entryParts[2]).put(key, entry.getValue());
      }
    }
    return target;
  }

  private final Map<String, String> values;
  private final Map<String, MapWithSubkeys> maps;

  public MapWithSubkeys() {
    this.values = new LinkedHashMap<String, String>();
    this.maps = new LinkedHashMap<String, MapWithSubkeys>();
  }

  public Collection<String> getKeys() {
    return values.keySet();
  }

  public Map<String, String> getValues() {
    return values;
  }

  public String getValueOf(String key) {
    return values.get(key);
  }

  public MapWithSubkeys get(String key) {
    if (!maps.containsKey(key)) {
      maps.put(key, new MapWithSubkeys());
    }
    if (!values.containsKey(key)) {
      put(key, null);
    }
    return maps.get(key);
  }

  public void put(String key, String value) {
    values.put(key, value);
  }

}
