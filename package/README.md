# Release tools

## Release model

The release model is linear. That means that there is only one main code branch, and releases are snapshots of that branch at specific points in the sequence of commits.
The advantage of this model is that the maintenance effort is greately reduced compared to other release models, such as a branched model.
The disadvantage is that progress is ever only forward, and fixes can't be backported to prior releases. this means there is no such thing as a "stable" release (however, see Sub releases below).

There are 4 types of releases:

*Major releases*

These contain breaking changes, such as big API changes, both in function signature and data representation. When this happens, user apps will require changes to continue to work, and those changes could be significant.
Major releases happen seldom, e.g.: every few years.
In addition, a Major can contain changes from Minor releases.

*Minor releases*

These contain new features and bug fixes. Breaking changes should not be included here. The one exception is breaking changes for a feature that is too broken and is not worth fixing, especially if that feature is causing maintenance overhead.
Minor releases happen maybe 1-3 times per year.
In addition, a Minor release can contain changes from Sub releases.

*Sub releases*

Sub releases are mostly meant for stabilization purposes. Once a Major or Minor release is out, it is possible that critical bugs or issues are found. Given that fixes can't be backported, a sub release is made that includes such critical bug fixes.
Sub releases happen a few weeks after a Major or Minor release.

*Beta releases*

Depending on the number of changes that have been merged since the last release, and on how big and disruptive those changes are, a beta release could be done prior to a Major or Minor. Beta releases are meant to provide an outlook of what the upcoming release will look like, in order to allow users to do early testing and provide feedback. This helps in identifying big issues early on, thereby allowing time to fix them before the final Major or Minor release.
Beta releases should not be done for Sub releases.

## Overview

This directory contains scripts used to automate the release process of esp8266/Arduino project.
The purpose of the release process is to generate the following outputs from the repository:

* Boards manager package index for Arduino IDE (i.e. `package_esp8266_index.json`). See [specification](https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.6.x-package_index.json-format-specification) of package index file for more info.

* Boards manager package for Arduino IDE. This is a .zip file which contains source code of esp8266/Arduino project, platform.txt, boards.txt and a few other supporting files. See [3-rd party hardware specification](https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.5-3rd-party-Hardware-specification) for more info about the layout of the boards manager package.

* Github Release for esp8266/Arduino project. This is used to host the boards manager package mentioned above, and also contains the release notes.

Here is an overview of the release process. See the section below for detailed instructions.

1. Release process starts when a maintainer pushes a tag into the repository.

2. Travis CI runs a build for this tag, and one of the jobs (with `BUILD_TYPE=package`) is used to prepare the boards manager package. This job runs `build_boards_manager_package.sh`.

3. `build_boards_manager_package.sh` does a few things to build the boards manager package (.zip) file and the json index:

   *  Pack source files into a zip file, excluding some files and tweaking platform.txt.
   *  Download current latest package index json file from Github Releases. This file contains descriptions of all previous releases.
   *  Generate package index for the new release.
   *  Combines new release with previous releases in one json file (using `merge_packages.py` script).

4. Travis CI uploads boards manager package (.zip file) and package index (.json file) to Github Releases, creating a draft release at the same time.

5. Travis CI also uploads package index .json file to `https://arduino.esp8266.com/stable/package_esp8266_index.json`, i.e. well-known URL used by most users.

6. When the draft release is created, maintainer edits release description and inserts changelog into the description field, unmarks the release as draft, and publishes the release.

7. Housekeeping is performed in Github issue tracker to close the completed milestone, update issue tags, etc.


## Creating a release (for maintainers)

0. Open a new issue to track activities, which will be closed after the release is done.

1. Assemble release notes.

    * Since most changes are integrated into master using squash-rebase policy (i.e. one commit per PR), `git log --oneline` gives a good overview of changes in the release.

    * Prepare release notes in Markdown format.

    * For changes that are breaking, duplicate those changes and put the duplicate lines into a separate group called Breaking Changes. That group should go at the top of the Changelog. The original lines for the breaking changes should be marked by appending "(Breaking change)" to the line. Example:
   
    ```
    Breaking Changes
    ================
    API xyz changed #1234
    ...

    Library - xyz
    =============
    API xyz changed #1234 (Breaking change)
    ...
    ```


    * Combine related changes into the following categories, in that order, including breaking changes with the appended mark:

      - Breaking Changes
      - Core
      - *Libraries* — one section per library that received changes. If library only had a single change or a few changes, it is also okay to combine changes to a few such libraries under single "Other Libraries" entry.
      - Upstream dependencies
      - Documentation
      - Boards

   * Not all commit descriptions which come from `git log` will explain changes well. Reword items as necessary, with the goal that a general user of this project should be able to understand what the change is related to. Preserve references to PRs or issues (`#XXXX`).

   * Don't include fixes for regressions which have been introduced since last release.

   * Aggregate minor fixes (e.g. typos, small documentation changes) in a few items. Focus on preparing a good overview of the release for the users, rather than mentioning every change.

   * When done, put release notes into a private Gist and send the link to other maintainers for review.

The following points assume work in a direct clone of the repository, and not in a personal fork.

2. Make a PR with the following, wait for Travis CI, and merge.

   * platform.txt: update `version` to the release in platform.txt E.g. `3.0.0`,

   * `cores/esp8266/TZ.h`: import the latest database with the following shell command: `$ cd <rootdir>; sh tools/TZupdate.sh` .

3. Tag the latest commit on the master branch. In this project, tags have form `X.Y.Z`, e.g. `3.0.0`, or `X.Y.Z-betaN` for release candiate versions. Notice that there's no `v`at the beginning of the tag. Tags must be annotated, not lightweight tags. To create a tag, use git command (assuming that the master branch is checked out):

   ```
   git tag -a -m "Release 3.0.0" 3.0.0
   ```

   then push the tag created in step 3 to esp8266/Arduino Github repository:

   ```
   git push origin 3.0.0
   ```

4. In case something goes wrong, release can be canceled at any time:

    * Tag must be removed (`git tag -d X.Y.Z; git push --delete origin X.Y.Z`)

    * Release must be deleted: github > releases > edit x.y.z > remove all files > delete button appears

5. Wait for Travis CI build for the tag to pass, see https://travis-ci.org/esp8266/Arduino/builds. 

6. Check that the new (draft) release has been created (no editing at this point!), see https://github.com/esp8266/Arduino/releases. Check that the boards manager package .zip file has been successfully uploaded as a release artifact.

7. Check that the package index downloaded from https://arduino.esp8266.com/stable/package_esp8266com_index.json contains an entry for the new version (it may not be the first one).

8. Navigate to release list in Github here https://github.com/esp8266/Arduino/releases, press "Edit" button to edit release description, paste release notes, and publish it.

9. In the issue tracker, remove "staged-for-release" label for all issues which have it, and close them. Close the milestone associated with the released version.

10. Check that https://arduino-esp8266.readthedocs.io/en/latest/ has a new doc build for the new tag, and that "stable" points to that build. If a new build did not trigger, log into readthedoc's home here https://readthedocs.org/ (account must have been added to project as maintainer) and trigger it manually.

11. Create a commit to the master branch, updating:

    * The version in platform.txt file. This should correspond to the version of the *next* milestone, plus `-dev` suffix. E.g. `3.1.0-dev`.

    * In main README.md:

        - in "Latest release" section, change version number in the readthedocs link to the version which was just released, and verify that all links work.
