This document describes rules that are in effect for this repository, meant for handling issues by contributors in the issue tracker and PRs.
# Opening New Issues
1.	The issue tracker is precisely that: a tool to track issues in the core code, and not a discussion forum. Opening an issue means that a problem has been found in the core code, and that it should be addressed by a contributor.
2.	When opening an issue, a template is presented with fields to fill out. The requested information is important. If the template is ignored, or not enough info about the issue is provided, the issue may be closed due to lack of info. Example:
      * Using WifiMulti and FS crashes with error. Why? (no basic info, no IDE settings, no sketch provided)
3. Questions of type "How do I..." or "Can you please help me with..." or "Can the ESP do..." won't be handled here. Such questions should be directed at a discussion forum, like esp8266.com or stackoverflow. All issues of this type will be closed with a simple reference to the policy. Example:
    * how do I connect to wifi
    * how do I connect two ESPs
    * can I send http data over a public network
    * my wiring/project/code doesn't work, help!
4. Issues that are obviously user error, programming language errors, lack of knowledge or experience with the use semantics of the core libs, or similar, will be closed with a reference to the policy. Examples: 
    *	sketch crashes due to a char[] in it that is not null terminated
    *	trying to use yield/delay, or libs that use yield/delay, from inside async callbacks
    *	Use of new/malloc without matching delete/free (mem leak)
5.	Issues about topics already handled in the documentation will be closed in a similar manner. Example:
    *	can't flash with error espcomm failed
6.	Issues must be provided with a minimalist sketch. Issues with an incomplete sketch, or a huge sketch, will be closed. Maximum effort must be put forth by the person opening the issue to reduce the relevant code that reproduces the issue, so that investigation can be taken up. MCVE is a must.
7. Issues for unmerged PRs will be closed. If there is an issue with a PR, the explanation should be added to the PR itself.
8. Issues with accompanied investigation that shows the root of the problem should be given priority
9.	Duplicate issues will be closed with a reference to the original

# Triaging
1.	Any contributor of the project can participate in the triaging process, if he/she chooses to do so
2.	An issue that needs to be closed, either due to not complying with this policy, or for other reasons, should be closed by a contributor
3.	Issues that are accepted should be marked with appropriate labels, e.g.: component: xyz
4.	If an issue is deemed to require specialized knowledge (e.g.: TLS, HTTP parser, SDK integration, etc), contributor(s) relevant to the affected code should be /cc’ed, as this can help grab attention
5.	Severe issues should be assigned to the current milestone (i.e.: the next version to be released), or the milestone following. It is ok to push back issues depending on available resources.
6.	Issues that could impact functionality for many users should be considered severe. 
7.	Issues caused by the SDK or chip should not be marked severe, as there usually isn’t much to be done. Common sense should be applied when deciding. Such issues should be documented in a KID (Known Issues Document), possibly on the Wiki, for reference by users. Example:
    * ARP issue
    * Extra channel change beacon announced by the SoftAP
    * Wakeup ROM bug in the ESP chip
8.	Issues with feature requests should be discussed for viability/desirability. Example:
    * Support for new board. If the new board is not widely used, doesn’t have a manufacturer webpage, etc, then it isn’t desirable to support it. If the new board is essentially a duplicate of another, it isn’t desirable to duplicate the existing one.
9.	Feature requests or changes that are meant to address a very specific/limited use case, especially if at the expense of increased code complexity, may be denied, or may be required to be redesigned, generalized, or simplified
10.	Feature requests that are not accompanied by a PR:
    * could be closed immediately (denied)
    * could be closed after some predetermined period of time (left as candidate for somebody to pick up)
    * could be deemed interesting enough to work on, but without a specific project or target, and hence accumulated in a long-term feature request list. Such feature requests will in general not be targeted for a deadline or release.
11.	In some cases, feedback may be requested from the issue reporter, either as additional info for clarification, additional testing, or other. If no feedback is provided after 30 days, the issue may be closed by a contributor.

# Compatibility
1.	Compatibility with the Arduino build system is first priority. Compatibility with PlatformIO and make are also maintained, but are second priority.
2.	Feature requests should consider compatibility with Arduino
    *	ESP-specific APIs should be added with care, and should be marked as such (Example: ESP8266WiFi)
    *	APIs of common libraries should maintain compatibility (Example: Wire, SPI, Servo)
    *	ESP-specific extensions to compatible APIs are ok, especially if required to fully use certain peripherals, but such functions should be clearly marked as ESP-specific
3.	When making changes that are likely to impact PlatformIO or make, relevant people should be notified. Check whether some corresponding changes are needed on the build system side. When an issue related to one of these build systems is reported, redirect the issue reporter to the respective issue tracker.
4.	The core libs are implemented as a wrapper layer over the Espressif SDK. Due to the requirements and limitations imposed by the SDK, there are inherent differences between the behavior of this core and the standard Arduino core (Example: using long delay()s is not allowed here). Compatibility can’t be maintained in such cases, and differences should be clearly documented.

# Pull requests
1.	All pull requests should undergo peer review by at least one contributor other than the creator
2.	All pull requests should consider updates to the documentation
3.	All pull requests should consider updates to regression tests, where possible
4.	Pull requests that address an outstanding issue, particularly an issue deemed to be severe, should be given priority
5.	If a PR is accepted, then it should undergo review and updated based on the feedback provided, then merged
6.   Pull requests that don't meet the above will be denied and closed

# Other
A table should be maintained for relating maintainers and components. When triaging, this is essential to figure out if someone in particular should be consulted about specific changes.

A stable release cadence should be established, e.g.: every 6 months.

Regression testing should be revisited and streamlined with the release process. Running regression tests should be done before merging a PR to reduce overhead for a release. 



