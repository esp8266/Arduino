package processing.app.windows;

import com.google.common.base.Predicate;
import com.google.common.collect.Iterables;

import java.util.Arrays;
import java.util.List;

public class RegQueryParser {

  private String valueOfKey;

  public RegQueryParser(String regQueryOutput) {
    parse(regQueryOutput);
  }

  private void parse(String regQueryOutput) {
    List<String> rows = Arrays.asList(regQueryOutput.replace("\r", "\n").replace("\n\n", "\n").split("\n"));

    String row = Iterables.find(rows, new Predicate<String>() {
      @Override
      public boolean apply(String input) {
        return input.startsWith("    ");
      }
    });

    String[] cols = row.split("    ");
    assert cols.length == 4;
    this.valueOfKey = cols[3];
  }

  public String getValueOfKey() {
    return valueOfKey;
  }
}
