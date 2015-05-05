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

package cc.arduino.contributions;

import cc.arduino.contributions.filters.BuiltInPredicate;
import cc.arduino.contributions.filters.InstalledPredicate;
import cc.arduino.contributions.packages.ContributedPackage;
import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.view.Event;
import com.google.common.base.Function;
import com.google.common.base.Predicates;
import com.google.common.collect.Collections2;
import com.google.common.collect.Iterables;
import com.google.common.collect.Lists;
import processing.app.Base;
import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.PreferencesData;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.util.LinkedList;
import java.util.List;

import static processing.app.I18n._;

public class BuiltInCoreIsNewerCheck implements Runnable {

  private final Base base;

  public BuiltInCoreIsNewerCheck(Base base) {
    this.base = base;
  }

  @Override
  public void run() {
    try {
      builtInPackageIsNewerCheck();
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  private void builtInPackageIsNewerCheck() throws InterruptedException {
    if (PreferencesData.getInteger("builtin_platform_is_newer", -1) >= BaseNoGui.REVISION) {
      return;
    }

    LinkedList<ContributedPlatform> contributedPlatforms = Lists.newLinkedList(Iterables.concat(Collections2.transform(BaseNoGui.indexer.getPackages(), new Function<ContributedPackage, List<ContributedPlatform>>() {
      @Override
      public List<ContributedPlatform> apply(ContributedPackage input) {
        return input.getPlatforms();
      }
    })));

    List<ContributedPlatform> installedBuiltInPlatforms = new LinkedList<ContributedPlatform>(Collections2.filter(contributedPlatforms, Predicates.and(new InstalledPredicate(), new BuiltInPredicate())));
    if (installedBuiltInPlatforms.size() != 1) {
      return;
    }
    final ContributedPlatform installedBuiltIn = installedBuiltInPlatforms.get(0);

    ContributedPlatform installedNotBuiltIn = BaseNoGui.indexer.getInstalled(installedBuiltIn.getParentPackage().getName(), installedBuiltIn.getArchitecture());
    if (installedNotBuiltIn == null) {
      return;
    }

    while (!base.hasActiveEditor()) {
      Thread.sleep(100);
    }

    if (VersionHelper.valueOf(installedBuiltIn.getParsedVersion()).greaterThan(VersionHelper.valueOf(installedNotBuiltIn.getParsedVersion()))) {
      SwingUtilities.invokeLater(new Runnable() {
        @Override
        public void run() {
          PreferencesData.setInteger("builtin_platform_is_newer", BaseNoGui.REVISION);
          assert base.hasActiveEditor();
          int chosenOption = JOptionPane.showConfirmDialog(base.getActiveEditor(), I18n.format(_("The IDE includes an updated {0} package, but you're using an older one.\nDo you want to upgrade {0}?"), installedBuiltIn.getName()), I18n.format(_("A newer {0} package is available"), installedBuiltIn.getName()), JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
          if (chosenOption == JOptionPane.YES_OPTION) {
            Action openBoardsManager = base.getOpenBoardsManager();
            Event event = new Event(base.getActiveEditor(), ActionEvent.ACTION_PERFORMED, installedBuiltIn.getName());
            event.getPayload().put("filterText", installedBuiltIn.getName());
            openBoardsManager.actionPerformed(event);
          }
        }
      });
    }
  }
}
