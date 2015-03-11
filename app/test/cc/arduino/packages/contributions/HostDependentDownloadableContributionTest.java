package cc.arduino.packages.contributions;

import org.junit.Test;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class HostDependentDownloadableContributionTest {

  @Test
  public void macOsXPositiveTest() {
    HostDependentDownloadableContributionStub contribution = new HostDependentDownloadableContributionStub() {
      @Override
      public String getHost() {
        return "x86_64-apple-darwin";
      }
    };
    System.setProperty("os.name", "Mac OS X");
    System.setProperty("os.arch", "x86_64");

    assertTrue(contribution.isCompatible());
  }

  @Test
  public void macOsXNegativeTest() {
    HostDependentDownloadableContributionStub contribution = new HostDependentDownloadableContributionStub() {
      @Override
      public String getHost() {
        return "x86_64-apple-darwin";
      }
    };
    System.setProperty("os.name", "Linux");
    System.setProperty("os.arch", "amd64");

    assertFalse(contribution.isCompatible());
  }

  @Test
  public void macOsXNegativeTest2() {
    HostDependentDownloadableContributionStub contribution = new HostDependentDownloadableContributionStub() {
      @Override
      public String getHost() {
        return "x86_64-apple-darwin";
      }
    };
    System.setProperty("os.name", "Mac OS X");
    System.setProperty("os.arch", "i686");

    assertFalse(contribution.isCompatible());
  }

}
