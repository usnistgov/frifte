FRIF E1N Validation
===================

We require exercising FRIF E1N [API] implementations with the FRIF E1N
validation package. Validation is mutually-beneficial to NIST and FRIF E1N
participants. The hope is that successful execution of validation ensures your
algorithm:

 * runs as expected at NIST;
 * does not crash;
 * syntactically and semantically implements the [API] correctly.

This helps cut the runtime of the evaluation and gives a higher level of
confidence that the results presented by NIST are a true measure of the
submitted software.

Contents
--------
 * Interaction Required:
   - **[config/]:** Directory in which all configuration files required by your
     libraries reside. This directory will be read-only at runtime. Use of
     configurations is optional.
   - **[lib/]:** Directory in which all required libraries reside. There must
     be at least library, the **core** library, and that library **must** follow
     the FRIF E1N naming convention.
   - **[../libfrifte_e1n/]:** Code for the shared library implementing methods
     declared in [../../include/frifte/e1n.h].
   - **[../../libfrifte/]:** Code for the shared library implementing methods
     shared by all FRIF evaluations, declared in [../../include/frif].
   - **[../../include/e1n.h]:** The FRIF E1N [API].
   - **[validate]:** Script that automates running the validation and performing
     checks on the output.
 * Supporting Files
   - **[CHECKSUMS]** Expected SHA-256 checksums of validation source files.
   - **[README.md]:** This file.
   - **[src/]:** Source code to build the validation executable.
   - **[VERSION]** Version number of the validation package.

Requirements
------------

 * Fingerprint Imagery
   - Because organizations must agree to NIST Special Database terms and
     conditions, the required fingerprint imagery is not included in this GitHub
     repository. Request and download the data from our [requests website].
 * Ubuntu Server 24.04.1 LTS
   - Even if this is not the latest version of Ubuntu Server, it will be the
     version used to run the evaluation. [Direct downloads] are available from
     the [Ubuntu Mirrors] and directly from the
     [NIST Biometric Technologies Group].
   - We **highly suggest** matching the exact versions of packages installed in
     our environment. A link to the names and versions of these pacakages is
     available.
   - The [validate] script  requires these base Ubuntu Server packages:
      - `base-files`, `binutils`, `cmake`, `coreutils`, `curl`, `dpkg`, `file`,
        `findutils`, `g++`, `gawk`, `grep`, `libc-bin`, `make`, `sed`, `tar`,
        `xz-utils`

It is **highly suggested** that you make sure your submission will build and run
as expected on environments as close as possible to the NIST evaluation
machines, in order to avoid validation delays. All Intel CPU types shall be
supported. Use of unavailable intrinsics shall degrade gracefully.

Be prepared to explain differences in templates and similarity scores. If at all
possible, please prevent differences due to hardware, lossy math and other
optimizations, and the like. Understand that NIST has a _right to reject_
submissions that cannot explain or correct differences.

How to Run
----------
 1. Put your compiled core library and any other required libraries in [lib/].
 2. Put any required configuration files in [config/].
 3. Put fingerprint imagery received from NIST in this directory (i.e.,
    the directory containing this file, [README.md]).
 4. Execute [validate] (`./validate`).
 5. **If successful**, sign *and* encrypt the resulting output archive in a
    single step, and upload it, along with the encrypting identity's public key,
    and your original signed evaluation agreement via [the FRIF upload form].
    For an example of how to use GnuPG to encrypt, run `validate encrypt`. If
    unsuccessful, correct any errors described and try again.

<details>
  <summary><em>Expand to view an example run.</em></summary>

```
$ bash
$ cp /path/to/libfrifte_e1n_nullimpl_0001.so lib/
$ cp /path/to/config.txt config/
$ cp /path/to/frif_e1n_validation_images-*.tar.xz .
$ ./validate
================================================================================
|   FRIF E1N Validation | Version 202409301547 | 30 Sep 2024 | 15:48:27 EDT    |
================================================================================
Checking for required packages... [OKAY]
Checking for previous validation attempts... [OKAY]
Checking validation version... (no Internet connection) [SKIP]
Checking OS and version... (Ubuntu Server 24.04.1 LTS (Noble Numbat)) [OKAY]
Checking for unexpanded validation image tarballs... [DEFER]
 -> Expanding "frif_e1n_validation_images-2024093014427.tar.xz"... [OKAY]
Checking for unexpanded validation image tarballs... [OKAY]
Checking validation image versions... (VERSION = 202409301442) [OKAY]
Looking for core library... (libfrifte_e1n_nullimpl_0001.so) [OKAY]
Checking for known environment variables... [OKAY]
Building... [OKAY]
Checking API version... [OKAY]
Checking library name... [OKAY]
Testing ExtractionInterface (probe)... [OKAY]
Merging createTemplate-probe-contactless-A logs... [OKAY]
Checking createTemplate contactless-A (probe) logs... [WARN]

================================================================================
| There are some (49) unexpected zero-byte probe templates. Please review:     |
| output/driver/createTemplate-probe-contactless-A.log                         |
================================================================================
Still checking createTemplate contactless-A (probe) logs... [OKAY]
Merging createTemplate-probe-palm-J logs... [OKAY]
Checking createTemplate palm-J (probe) logs... [WARN]

================================================================================
| There are some (22) unexpected zero-byte probe templates. Please review:     |
| output/driver/createTemplate-probe-palm-J.log                                |
================================================================================
Still checking createTemplate palm-J (probe) logs... [OKAY]
Merging createTemplate-probe-plain-M logs... [OKAY]
Checking createTemplate plain-M (probe) logs... [WARN]

================================================================================
| There are some (100) unexpected zero-byte probe templates. Please review:    |
| output/driver/createTemplate-probe-plain-M.log                               |
================================================================================
Still checking createTemplate plain-M (probe) logs... [OKAY]
Merging createTemplate-probe-roll-V logs... [OKAY]
Checking createTemplate roll-V (probe) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte probe templates. Please review:     |
| output/driver/createTemplate-probe-roll-V.log                                |
================================================================================
Still checking createTemplate roll-V (probe) logs... [OKAY]
Merging createTemplate-probe-slap-R logs... [OKAY]
Checking createTemplate slap-R (probe) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte probe templates. Please review:     |
| output/driver/createTemplate-probe-slap-R.log                                |
================================================================================
Still checking createTemplate slap-R (probe) logs... [OKAY]
Skipping extract template data (probe) checks
Testing ExtractionInterface (reference)... [OKAY]
Merging createTemplate-reference-palm-N logs... [OKAY]
Checking createTemplate palm-N (reference) logs... [WARN]

================================================================================
| There are some (49) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-palm-N.log                            |
================================================================================
Still checking createTemplate palm-N (reference) logs... [OKAY]
Merging createTemplate-reference-plain-M logs... [OKAY]
Checking createTemplate plain-M (reference) logs... [WARN]

================================================================================
| There are some (100) unexpected zero-byte reference templates. Please        |
| review:                                                                      |
| output/driver/createTemplate-reference-plain-M.log                           |
================================================================================
Still checking createTemplate plain-M (reference) logs... [OKAY]
Merging createTemplate-reference-roll-U logs... [OKAY]
Checking createTemplate roll-U (reference) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-roll-U.log                            |
================================================================================
Still checking createTemplate roll-U (reference) logs... [OKAY]
Merging createTemplate-reference-roll-U-palm-N logs... [OKAY]
Checking createTemplate roll-U-palm-N (reference) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-roll-U-palm-N.log                     |
================================================================================
Still checking createTemplate roll-U-palm-N (reference) logs... [OKAY]
Merging createTemplate-reference-roll-U-roll-V-slap-R logs... [OKAY]
Checking createTemplate roll-U-roll-V-slap-R (reference) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-roll-U-roll-V-slap-R.log              |
================================================================================
Still checking createTemplate roll-U-roll-V-slap-R (reference) logs... [OKAY]
Merging createTemplate-reference-segmented-slap-R logs... [OKAY]
Checking createTemplate segmented-slap-R (reference) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-segmented-slap-R.log                  |
================================================================================
Still checking createTemplate segmented-slap-R (reference) logs... [OKAY]
Merging createTemplate-reference-slap-R-contactless-A logs... [OKAY]
Checking createTemplate slap-R-contactless-A (reference) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-slap-R-contactless-A.log              |
================================================================================
Still checking createTemplate slap-R-contactless-A (reference) logs... [OKAY]
Merging createTemplate-reference-slap-R-roll-U logs... [OKAY]
Checking createTemplate slap-R-roll-U (reference) logs... [WARN]

================================================================================
| There are some (50) unexpected zero-byte reference templates. Please         |
| review:                                                                      |
| output/driver/createTemplate-reference-slap-R-roll-U.log                     |
================================================================================
Still checking createTemplate slap-R-roll-U (reference) logs... [OKAY]
Skipping extract template data (reference) checks
Testing reference database creation... [OKAY]
Testing SearchInterface (subject)... [OKAY]
Merging search-subject-palm-J_v_palm-N logs... [OKAY]
Checking search subject palm-J_v_palm-N log (candidates)... [OKAY]
Merging search-subject-slap-R_v_roll-U logs... [OKAY]
Checking search subject slap-R_v_roll-U log (candidates)... [OKAY]
Testing SearchInterface (subject+position)... [OKAY]
Merging search-subject+position-contactless-A_v_palm-N logs... [OKAY]
Checking search subject+position contactless-A_v_palm-N log (candidates)... [OKAY]
Merging search-subject+position-roll-V_v_roll-U logs... [OKAY]
Checking search subject+position roll-V_v_roll-U log (candidates)... [OKAY]
Merging search-subject+position-roll-V_v_slap-R-contactless-A logs... [OKAY]
Checking search subject+position roll-V_v_slap-R-contactless-A log (candidates)... [OKAY]
Merging search-subject+position-plain-M_v_roll-U-roll-V-slap-R logs... [OKAY]
Checking search subject+position plain-M_v_roll-U-roll-V-slap-R log (candidates)... [OKAY]
Creating validation submission... (frif_e1n_validation_nullimpl_0001.tar.xz) [OKAY]

================================================================================
| You did not implement one or more of the methods that provides insight into  |
| your algorithm's decision making. While these methods are optional, they     |
| will greatly assist in forensic science research activities at NIST. If      |
| this is unexpected, please review the log output.                            |
|                                                                              |
| * extractTemplateData (probes): NOT implemented                              |
| * extractTemplateData (references): NOT implemented                          |
| * extractCorrespondence: NOT implemented                                     |
================================================================================

================================================================================
| Please review the marketing and CBEFF information compiled into your         |
| library to ensure correctness:                                               |
|                                                                              |
| Exemplar Feature Extraction Algorithm Marketing Identifier =                 |
| NullImplementation Exemplar Extractor 1.0                                    |
| Exemplar Feature Extraction Algorithm CBEFF Owner = 0x000F                   |
| Exemplar Feature Extraction Algorithm CBEFF Identifier = 0xF1A7              |
| Search Algorithm Marketing Identifier = NullImplementation Comparator (N     |
| 1:1 mode) 1.0                                                                |
| Search Algorithm CBEFF Owner = 0x000F                                        |
| Search Algorithm CBEFF Identifier = 0x0101                                   |
================================================================================

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ This script could not check online to ensure there are no updates            +
+ available. NIST requires that FRIF submissions always use the latest         +
+ version. Retrieve the latest version number by visiting the URL below and    +
+ be sure it matches this version: 202405221259.                               +
+                                                                              +
+ https://github.com/usnistgov/frifte/tree/main/e1n/validation/VERSION           +
+                                                                              +
+ If these numbers don't match, visit our website to retrieve the latest       +
+ version.                                                                     +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

================================================================================
| You have successfully completed your part of FRIF E1N validation. Please     |
| sign and encrypt the file listed below (run './validate encrypt' for an      |
| example).                                                                    |
|                                                                              |
|                   frif_e1n_validation_nullimpl_0001.tar.xz                   |
|                                                                              |
| Please upload both frif_e1n_validation_nullimpl_0001.tar.xz.asc and your     |
| public key via https://pages.nist.gov/frifte/upload                            |
================================================================================
Completed: 30 Sep 2024 | 13:57:37 EDT (Runtime: 379s)
```
</details>

Submission Contents
-------------------

 * **canary.log:** MD5 checksums of randomly-generated images we provide as part
   of the validation fingerprint imagery. This helps us make sure that you are
   using the most recent version of FRIF E1N validation fingerprint imagery.
 * **compile.log:** Output from compiling the validation executable and other
   information like MD5 checksums, versions, and library dependencies that may
   help us debug your submission if an error occurs.
 * **config/:** A copy of [config/].
 * **driver/:** Output from the validation executable:
   * **createDatabase-*.log:** Output from creating the named reference
     database.
   * **createTemplate-*.log:** Output from generating the named templates.
   * **extractCorrespondence-*.log**: Corresponding features from the named
     searches, if supported.
   * **extractTemplateData-*.log:** Features extracted from the named templates,
     if supported.
   * **reference_databases/:** Reference databases created.
   * **search-*.log:** Output from named search operations.
   * **templates/:** Templates created
 * **id_extraction.log:** Information about the linked core library's
   `ExtractionInterface`.
 * **id.log:** Information about the linked core library.
 * **id_search.log:** Information about the linked core library's
   `SearchInterface`.
 * **lib/:** A copy of [lib/].
 * **run-createReferenceDatabases.log:** The command used to launch the
   validation executable when generating `driver/createDatabase-*.log`.
 * **run-extract-*.log:** The command used to launch the validation
   executable when generating `driver/createTemplate-*.log`.
 * **run-search-*.log:** The command used to launch the
   validation executable when generating `driver/search-*.log`.

### A Note about Paths
This following  files may contain absolute paths to files on your system. If
this information is sensitive, you may redact the paths, leaving the structure
of the file content the same.

 * canary.log
 * compile.log
 * run-*.log

Checks Performed
----------------

 * Implementation can handle being `fork`ed.
 * Validation package and imagery is at most recent revision level.
 * Appropriate operating system version installed.
 * Libraries and configurations can be placed randomly on disk.
 * Appropriately-named FRIF E1N core software library is present.
 * Software library links properly against the validation driver.
 * Crashes do not occur when handling various types of imagery, including
   - atypical resolutions;
   - atypical data sources (e.g., features-only, latent probe)
   - unknown metadata;
   - non-contact imagery;
   - blank or gradient patterns.

While the validation package tries to eliminate errors from the FRIF E1N
submission, it is not perfect, and there are still several ways in which the
package might approve you for submission that NIST may later reject.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please [open an issue]. Other questions may be addressed
to the [NIST FRIF TE team].

The FRIF TE team sends updates about the FRIF tests to their mailing list. Enter
your e-mail address on the [mailing list site], or send a blank e-mail to
FRIFTE+subscribe@list.nist.gov to be automatically subscribed.

License
-------
The items in this repository are released in the public domain. See the
[LICENSE] for details.

[API]: https://pages.nist.gov/frifte/doc/api/e1n.html
[Ubuntu Mirrors]: https://launchpad.net/ubuntu/+cdmirrors
[Direct downloads]: https://releases.ubuntu.com/releases/24.04.1/ubuntu-24.04.1-live-server-amd64.iso
[NIST Biometric Technologies Group]: https://nigos.nist.gov/evaluations/ubuntu-24.04.1-live-server-amd64.iso
[lib/]: https://github.com/usnistgov/frifte/tree/main/e1n/validation/lib
[../libfrifte_e1n/]: https://github.com/usnistgov/frifte/tree/main/e1n/libfrifte_e1n
[../../libfrifte/]: https://github.com/usnistgov/frifte/tree/main/libfrifte
[../../include/e1n.h]: https://github.com/usnistgov/frifte/blob/main/include/frifte/e1n.h
[bin/]: https://github.com/usnistgov/frifte/tree/main/e1n/validation/bin
[config/]: https://github.com/usnistgov/frifte/tree/main/e1n/validation/config
[README.md]: https://github.com/usnistgov/frifte/blob/main/e1n/validation/README.md
[src/]: https://github.com/usnistgov/frifte/tree/main/e1n/validation/src
[CHECKSUMS]: https://github.com/usnistgov/frifte/blob/main/e1n/validation/CHECKSUMS
[VERSION]: https://github.com/usnistgov/frifte/tree/main/e1n/validation/VERSION
[validate]: https://github.com/usnistgov/frifte/tree/main/e1n/validation/validate
[NIST FRIF TE team]: mailto:frifte@nist.gov
[open an issue]: https://github.com/usnistgov/frifte/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/g/frif
[LICENSE]: https://github.com/usnistgov/frifte/blob/main/LICENSE.md
[test plan]: https://pages.nist.gov/frifte/doc/testplan/e1n_testplan.pdf
[requests website]: https://nigos.nist.gov/datasets/frif_e1n_validation/request
[the FRIF upload form]: https://pages.nist.gov/frifte/e1n/upload
[../../include/frifte]: https://github.com/usnistgov/frifte/tree/main/include/frifte
[../../include/frifte/e1n.h]: https://github.com/usnistgov/frifte/blob/main/include/frifte/e1n.h
