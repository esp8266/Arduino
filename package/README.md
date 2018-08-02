# Release tools

## Overview

This directory contains scripts used to automate the release process of esp8266/Arduino project.
The purpose of the release process is to generate the following outputs from the repository:

* Boards manager package index for Arduino IDE (i.e. `package_esp8266_index.json`). See [specification](https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.6.x-package_index.json-format-specification) of package index file for more info.

* Boards manager package for Arduino IDE. This is a .zip file which contains source code of esp8266/Arduino project, platform.txt, boards.txt and a few other supporting files. See [3-rd party hardware specification](https://github.com/arduino/Arduino/wiki/Arduino-IDE-1.5-3rd-party-Hardware-specification) for more info about the layout of the boards manager package.

* Github Release for esp8266/Arduino project. This is used to host the boards manager package mentioned above, and also contains the release notes.

Here is an overview of the release process.

1. Release process starts when a maintainer pushes a tag into the repository.

2. Travis CI runs a build for this tag, and one of the jobs (with `BUILD_TYPE=package`) is used to prepare the boards manager package. This job runs `build_boards_manager_package.sh`.

3. `build_boards_manager_package.sh` does a few things to build the boards manager package (.zip) file and the json index:

   *  Pack source files into a zip file, excluding some files and tweaking platform.txt.
   *  Download current latest package index json file from Github Releases. This file contains descriptions of all previous releases.
   *  Generate package index for the new release.
   *  Combines new release with previous releases in one json file (using `merge_packages.py` script).

4. Travis CI uploads boards manager package (.zip file) and package index (.json file) to Github Releases, creating a draft release at the same time.

5. Travis CI also uploads package index .json file to `http://arduino.esp8266.com/stable/package_esp8266_index.json`, i.e. well-known URL used by most users. 

6. When the draft release is created, maintainer edits release description and inserts changelog into the description field, unmarks the release as draft, and publishes the release.

7. Housekeeping is performed in Github issue tracker to close the completed milestone, update issue tags, etc.


## Creating a release (for maintainers)

1. Assemble release notes.

   * Since most changes are integrated into master using squash-rebase policy (i.e. one commit per PR), `git log --oneline` gives a good overview of changes in the release.

   * Prepare release notes in Markdown format.

   * Combine related changes into the following categories, in that order:

      - Core
      - *Libraries* — one section per library that received changes. If library only had a single change or a few changes, it is also okay to combine changes to a few such libraries under single "Other Libraries" entry.
      - Upstream dependencies
      - Documentation
      - Boards

   * Not all commit descriptions which come from `git log` will explain changes well. Reword items as necessary, with the goal that a general user of this project should be able to understand what the change is related to. Preserve references to PRs or issues (`#XXXX`).

   * Don't include fixes for issues which have been introduced since last release.

   * Aggregate minor fixes (e.g. typos, small documentation changes) in a few items. Focus on preparing a good overview of the release for the users, rather than mentioning every change.

   * When done, put release notes into a private Gist and send the link to other maintainers for review.

2. Tag the latest commit. In this project, tags have form `X.Y.Z`, e.g. `2.4.0`, or `X.Y.Z-rcN` for release versions. Notice that there's no `v`at the beginning of the tag. Tags must be annotated, not lightweight tags. To create a tag, use git command (assuming that the master branch is checked out):

   ```
   git tag -a -m "Release 2.4.0" 2.4.0
   ```
   
   Push the tag to esp8266/Arduino Github repository.


3. Wait for Travis CI build for the tag to pass. Check that the new (draft) release has been created. Check that the boards manager package .zip file has been successfully uploaded as a release artifact.

4. Check that the package index downloaded from http://arduino.esp8266.com/stable/package_esp8266_index.json contains an entry for the new version (it may not be the first one).

5. Navigate to release list in Github, press "Edit" button to edit release description, paste release notes. Remove "draft" status of the release and publish it.

6. In the issue tracker, remove "staged-for-release" label for all issues which have it, and close them. Close the milestone associated with the released version.

7. Make a commit to master branch, updating the version in platform.txt file. This should correspond to the version of the *next* milestone.


