package processing.app.helpers;

import java.util.Map;
import java.util.Map.Entry;

public class Maps {

  public static <K, V> Map<K, V> merge(Map<K, V> input, Map<K, V> target) {
    for (Entry<K, V> entry : input.entrySet()) {
      target.put(entry.getKey(), entry.getValue());
    }
    return target;
  }
  
}
