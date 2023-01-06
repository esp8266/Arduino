# Release tools

## Release model

The release model is linear. That means that there is only one main code branch, and releases are snapshots of that branch at specific points in the sequence of commits.
The advantage of this model is that the maintenance effort is greatly reduced compared to other release models, such as a branched model.
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

Here is a rough overview of the effective release process. See the section below for detailed instructions.

1. Release process effectively starts when a maintainer pushes a tag into the repository.

2. CI runs a build for this tag, and one of the jobs is used to prepare the boards manager package. This job runs `build_boards_manager_package.sh`.

3. `build_boards_manager_package.sh` does a few things to build the boards manager package (.zip) file and the json index:

   *  Pack source files into a zip file, excluding some files and tweaking platform.txt.
   *  Download current latest package index json file from Github Releases. This file contains descriptions of all previous releases.
   *  Generate package index for the new release.
   *  Combines new release with previous releases in one json file (using `merge_packages.py` script).

4. CI uploads boards manager package (.zip file) and package index (.json file) to Github Releases, creating a draft release at the same time.

5. CI also uploads package index .json file to `https://arduino.esp8266.com/stable/package_esp8266_index.json`, i.e. well-known URL used by most users.

6. When the draft release is created, maintainer edits release description and inserts changelog into the description field, unmarks the release as draft, and publishes the release.

7. Housekeeping is performed in Github issue tracker to close the completed milestone, update issue tags, etc.


## Creating a release (for maintainers)

1. Make sure that no issues or PRs are assigned to the milestone to be released. If there are any Issues/PRs assigned to the relevant milestone, they should either be addressed, pushed back to a future milestone, or closed.

2. Open a new issue to track activities, which will be closed after the release is done. Copy the checklist below into it, and check the steps one by one as they get completed.

3. Assemble release notes.

    * Since most changes are integrated into master using squash-rebase policy (i.e. one commit per PR), `git log --oneline` gives a good overview of changes in the release.

    * Prepare release notes in Markdown format. Either use the `git log --oneline` output and sort through it manually, or use Github draft release and press 'Generate release notes' (see https://docs.github.com/en/repositories/releasing-projects-on-github/automatically-generated-release-notes)

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

   * Not all commit descriptions which come from `git log` or PR titles will explain changes well. Reword items as necessary, with the goal that a general user of this project should be able to understand what the change is related to. Preserve references to PRs or issues (`#XXXX`).

   * Aggregate minor fixes (e.g. typos, small documentation changes) in a few items. Focus on preparing a good overview of the release for the users, rather than mentioning every change.

   * When done, put release notes into a private [Gist](https://gist.github.com) or [HedgeDoc note](https://hedgedoc.org/) and send the link to other maintainers for review.

The following points assume work in a direct clone of the repository, and not in a personal fork.

4. Make a PR with the following, wait for CI, and merge.

   * [platform.txt](https://github.com/esp8266/Arduino/blob/master/platform.txt) and [package.json](https://github.com/esp8266/Arduino/blob/master/package.json): update `version` to the release E.g. `3.0.0`,

   * [cores/esp8266/TZ.h](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h): import the latest database with the following shell command:\
     `$ cd tools; sh TZupdate.sh`

   * Update SSL/TLS certificates and public keys in examples:\
     `$ cd tools; sh certsUpdate.sh`

5. Wait until the release notes have been checked by other maintainers

6. Tag the latest commit on the master branch. In this project, tags have form `X.Y.Z`, e.g. `3.0.0`, or `X.Y.Z-betaN` for release candidate versions. Notice that there's no `v`at the beginning of the tag. Tags must be annotated, not lightweight tags. To create a tag, use git command (assuming that the master branch is checked out):

   ```
   git tag -a -m "Release 3.0.0" 3.0.0
   ```

   push the tag created above to esp8266/Arduino Github repository:

   ```
   git push origin 3.0.0
   ```

   In case something goes wrong, release can be canceled at any time:

    * Tag must be removed (`git tag -d X.Y.Z; git push --delete origin X.Y.Z`)

    * Release must be deleted: github > releases > edit x.y.z > remove all files > delete button appears

7. Wait for CI build to pass

8. Check that the new (draft) release has been created (no editing at this point!), see https://github.com/esp8266/Arduino/releases.

9. Check that the boards manager package .zip file has been successfully uploaded as a release asset.

10. Check that the package index downloaded from https://arduino.esp8266.com/stable/package_esp8266com_index.json contains an entry for the new version (it may not be the first one).

11. Navigate to release list in Github here https://github.com/esp8266/Arduino/releases, press "Edit" button to edit release description, paste release notes, and publish it.

12. Close the milestone associated with the released version (the milestone should be empty per point 1 above)

13. Check that https://arduino-esp8266.readthedocs.io/en/latest/ has a new doc build for the new tag, and that "stable" points to that build. If a new build did not trigger, log into readthedoc's home here https://readthedocs.org/ (account must have been added to project as maintainer) and trigger it manually.

14. Create a commit to the master branch, updating:

    * The version in platform.txt and package.json files. This should correspond to the version of the *next* milestone, plus `-dev` suffix. E.g. `3.1.0-dev`.

    * In main README.md go to "Latest release" section, change version number in the readthedocs link to the version which was just released, and verify that all links work.


## Checklist helper, copy-paste into Release Process issue
```
--------------COPY BELOW THIS LINE--------------
[Reference](https://github.com/esp8266/Arduino/tree/master/package#creating-a-release-for-maintainers) for details.

- [ ] 1. Make sure that no issues or PRs are assigned to the milestone to be released.

- [ ] 2. Open a new issue to track activities.

- [ ] 3. Assemble release notes.

- [ ] 4. Make a PR with the following, [wait for CI](https://github.com/esp8266/Arduino/pull/8034/checks), and merge.

   * [platform.txt](https://github.com/esp8266/Arduino/blob/master/platform.txt)
   * [package.json](https://github.com/esp8266/Arduino/blob/master/package.json)
   * [TZ.h](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h) (<= `cd tools; sh ./TZupdate.sh`)
   * Certificates (<= `cd tools; sh certsUpdate.sh`)

- [ ] 5. Wait until the release notes have been checked by other maintainers (can be changed afterwards anyway)

- [ ] 6. Tag the latest commit on the master branch, then push it to esp8266/Arduino
      add: `git tag -a -m "Release 3.0.0" 3.0.0; git push origin 3.0.0`
      remove: `git tag -d X.Y.Z; git push --delete origin X.Y.Z`

- [ ] 7. Wait for CI build for the tag to pass (in `Actions` menu)

- [ ] 8. Check that the new (draft) release has been created (no editing at this point!), see https://github.com/esp8266/Arduino/releases.

- [ ] 9. Check that the boards manager package .zip file has been successfully uploaded as a release asset.

- [ ] 10. Check that the package index downloaded from https://arduino.esp8266.com/stable/package_esp8266com_index.json contains an entry for the new version (it may not be the first one).

- [ ] 11. Navigate to [release list in Github](https://github.com/esp8266/Arduino/releases), press "Edit" button to edit release description, paste release notes, and publish it.

- [ ] 12. Close the milestone associated with the released version (the milestone should be empty per point 1 above)

- [ ] 13. Check that https://arduino-esp8266.readthedocs.io/en/latest/ has a new doc build for the new tag, and that "stable" points to that build. If a new build did not trigger, log into readthedoc's home here https://readthedocs.org/ (account must have been added to project as maintainer) and trigger it manually.

- [ ] 14. Create a commit to the master branch, updating:

    * The version in platform.txt and package.json files. This should correspond to the version of the *next* milestone, plus `-dev` suffix. E.g. `3.1.0-dev`.
    * In main README.md go to "Latest release" section, change version number in the readthedocs link to the version which was just released, and verify that all links work.
--------------COPY ABOVE THIS LINE--------------
```

## Updating a SSH deploy key

A SSH private/public key pair is required to update the master JSON (the final step of the release process).  Sometimes GitHub will expire one side or the other of that key, and a new one will need to be regenerated and installed in the https://github.com/esp8266/esp8266.github.io (JSON) and https://github.com/esp8266/Arduino (core) repos.

1. Generate a new public/private SSH key pair with an empty passphrase:
```console
$ ssh-keygen -f deploy_key -t ed25519 -N '' -C earlephilhower@yahoo.com  (**replace with your GH user account email**)
Generating public/private ed25519 key pair.
Your identification has been saved in deploy_key
Your public key has been saved in deploy_key.pub
The key fingerprint is:
...
```

2. Copy the contents of `deploy_key.pub` to the clipboard:
```console
$ cat deploy_key.pub
ssh-ed25519 AAA..... earlephilhower@yahoo.com
```

3. Install the deploy key for esp8266.github.io repository.  Go to https://github.com/esp8266/esp8266.github.io and the `Settings->Deploy Keys` and `Add deploy key`.  Paste the (public key) string into the box and select `Allow writes` and hit OK.

4. Convert the `deploy_key` private key to a 1-line base64 representation and copy it to the clipboard.
```console
$ base64 -w 0 < deploy_key && echo ""
yEvYm.....  (**note this must be one single long line, hence the "-w 0"**)
```

5. Install the private key to the Core repo.  Go to https://github.com/esp8266/Arduino and select `Settings->Secrets->Actions` and add or update a `Repository secret` called `GHCI_DEPLOY_KEY`.  Paste the 1-line base64 contents of your clipboard to the box and hit OK.

6. If the release failed in the `Update master JSON file` action, from the GitHub web interface run the `Actions->Release XXX->Re-run failed jobs` to re-run it and check its output.
