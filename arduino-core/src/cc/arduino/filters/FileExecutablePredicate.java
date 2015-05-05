package cc.arduino.filters;

import com.google.common.base.Predicate;

import java.io.File;

public class FileExecutablePredicate implements Predicate<File> {

  @Override
  public boolean apply(File file) {
    return file.isFile() && file.exists() && file.canRead() && file.canExecute();
  }

}
