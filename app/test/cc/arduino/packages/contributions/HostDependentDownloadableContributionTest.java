/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package cc.arduino.packages.contributions;

import org.junit.Test;
import processing.app.Platform;

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

    Platform platform = new Platform() {
      @Override
      public String getOsName() {
        return "Mac OS X";
      }

      @Override
      public String getOsArch() {
        return "x86_64";
      }
    };

    assertTrue(contribution.isCompatible(platform));
  }

  @Test
  public void macOsXNegativeTest() {
    HostDependentDownloadableContributionStub contribution = new HostDependentDownloadableContributionStub() {
      @Override
      public String getHost() {
        return "x86_64-apple-darwin";
      }
    };

    Platform platform = new Platform() {
      @Override
      public String getOsName() {
        return "Linux";
      }

      @Override
      public String getOsArch() {
        return "amd64";
      }
    };

    assertFalse(contribution.isCompatible(platform));
  }

  @Test
  public void macOsXNegativeTest2() {
    HostDependentDownloadableContributionStub contribution = new HostDependentDownloadableContributionStub() {
      @Override
      public String getHost() {
        return "x86_64-apple-darwin";
      }
    };

    Platform platform = new Platform() {
      @Override
      public String getOsName() {
        return "Mac OS X";
      }

      @Override
      public String getOsArch() {
        return "i686";
      }
    };

    assertFalse(contribution.isCompatible(platform));
  }

}
