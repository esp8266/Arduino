package cc.arduino.contributions.libraries.filters;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.packages.ContributionsIndexer;
import com.google.common.base.Predicate;

public class LibraryInstalledInsideCore implements Predicate<ContributedLibrary> {

  private final ContributionsIndexer indexer;

  public LibraryInstalledInsideCore(ContributionsIndexer indexer) {
    this.indexer = indexer;
  }

  @Override
  public boolean apply(ContributedLibrary contributedLibrary) {
    return indexer.isFolderInsidePlatform(contributedLibrary.getInstalledFolder());
  }

}
