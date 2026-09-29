FRIF TE Quality Validation
==========================

## Submission Window Closed
NIST is not currently accepting submissions. We hope to begin toward
**late winter 2026/2027**. Thanks for your patience.

About
-----

We require exercising FRIF TE Quality [API] implementations with the FRIF TE
Quality validation package. Validation is mutually-beneficial to NIST and
FRIF TE Quality participants. The hope is that successful execution of
validation ensures your algorithm:

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
     the FRIF TE Quality naming convention.
   - **[../libfrifte_quality/]:** Code for the shared library implementing
     methods declared in [../../include/frifte/quality.h].
   - **[../../libfrifte/]:** Code for the shared library implementing methods
     shared by all FRIF TE evaluations, declared in [../../include/frifte].
   - **[../../include/quality.h]:** The FRIF TE Quality [API].
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
 * Ubuntu Server 24.04.3 LTS
   - Even if this is not the latest version of Ubuntu Server, it will be the
     version used to run the evaluation. Direct downloads are available from the
     [Ubuntu Mirrors] ([🇺🇸 USA], [🇪🇺 Europe]) and directly from the [NIST
     Image Group].
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
    and your original signed evaluation agreement via [the FRIF TE upload form].
    For an example of how to use GnuPG to encrypt, run `validate encrypt`. If
    unsuccessful, correct any errors described and try again.

<details>
  <summary><em>Expand to view an example run.</em></summary>

```
$ bash
$ cp /path/to/libfrifte_quality_nullimpl_0001.so lib/
$ cp /path/to/config.txt config/
$ cp /path/to/frif_quality_validation_images-*.tar.xz .
$ ./validate
================================================================================
|    FRIF TE Quality Validation | 202509240859 | 28 Sep 2026 | 15:18:48 EDT    |
================================================================================
Checking for required packages... [OKAY]
Checking for previous validation attempts... [OKAY]
Checking validation version... (no Internet connection) [SKIP]
Checking OS and version... (Ubuntu Server 24.04.3 LTS (Noble Numbat)) [OKAY]
Checking for unexpanded validation image tarballs... [OKAY]
Checking validation image versions... (VERSION = 202609240855) [OKAY]
Looking for core library... (libfrifte_quality_nullimpl_0001.so) [OKAY]
Checking for known environment variables... [OKAY]
Building... [OKAY]
Checking API version... [OKAY]
Checking library name... [OKAY]
Running QualityInterface::computeUnifiedQuality() for DistalExemplar... [OKAY]
 ├ Merging unified-DistalExemplar logs... [OKAY]
 └ Checking compute unified quality (DistalExemplar) logs... [WARN]

================================================================================
| There were 12 failures to compute quality.                                   |
| output/driver/unified-DistalExemplar.log                                     |
================================================================================
Running QualityInterface::computeUnifiedQuality() for DistalLatent... [OKAY]
 ├ Merging unified-DistalLatent logs... [OKAY]
 └ Checking compute unified quality (DistalLatent) logs... [WARN]

================================================================================
| There were 3 failures to compute quality.                                    |
| output/driver/unified-DistalLatent.log                                       |
================================================================================
Running QualityInterface::computeUnifiedQuality() for NonDistalExemplar... [OKAY]
 ├ Merging unified-NonDistalExemplar logs... [OKAY]
 └ Checking compute unified quality (NonDistalExemplar) logs... [OKAY]
Running QualityInterface::computeUnifiedQuality() for NonDistalLatent... [OKAY]
 ├ Merging unified-NonDistalLatent logs... [OKAY]
 └ Checking compute unified quality (NonDistalLatent) logs... [OKAY]
Running QualityInterface::computeUnifiedQuality() for PalmExemplar... [OKAY]
 ├ Merging unified-PalmExemplar logs... [OKAY]
 └ Checking compute unified quality (PalmExemplar) logs... [OKAY]
Running QualityInterface::computeUnifiedQuality() for PalmLatent... [OKAY]
 ├ Merging unified-PalmLatent logs... [OKAY]
 └ Checking compute unified quality (PalmLatent) logs... [OKAY]
Running QualityInterface::computeUnifiedQuality() for UnknownLatent... [OKAY]
 ├ Merging unified-UnknownLatent logs... [OKAY]
 └ Checking compute unified quality (UnknownLatent) logs... [WARN]

================================================================================
| There was 1 failure to compute quality.                                      |
| output/driver/unified-UnknownLatent.log                                      |
================================================================================
Running QualityInterface::computeVerboseQuality() for DistalExemplar... [OKAY]
 ├ Merging verbose-DistalExemplar logs... [OKAY]
 └ Checking compute verbose quality (DistalExemplar) logs... [WARN]

================================================================================
| There were 13 failures to compute unified quality.                           |
| output/driver/verbose-DistalExemplar.log                                     |
================================================================================
Running QualityInterface::computeVerboseQuality() for DistalLatent... [OKAY]
 ├ Merging verbose-DistalLatent logs... [OKAY]
 └ Checking compute verbose quality (DistalLatent) logs... [WARN]

================================================================================
| There were 3 failures to compute unified quality.                            |
| output/driver/verbose-DistalLatent.log                                       |
================================================================================
Running QualityInterface::computeVerboseQuality() for NonDistalExemplar... [OKAY]
 ├ Merging verbose-NonDistalExemplar logs... [OKAY]
 └ Checking compute verbose quality (NonDistalExemplar) logs... [OKAY]
Running QualityInterface::computeVerboseQuality() for NonDistalLatent... [OKAY]
 ├ Merging verbose-NonDistalLatent logs... [OKAY]
 └ Checking compute verbose quality (NonDistalLatent) logs... [OKAY]
Running QualityInterface::computeVerboseQuality() for PalmExemplar... [OKAY]
 ├ Merging verbose-PalmExemplar logs... [OKAY]
 └ Checking compute verbose quality (PalmExemplar) logs... [OKAY]
Running QualityInterface::computeVerboseQuality() for PalmLatent... [OKAY]
 ├ Merging verbose-PalmLatent logs... [OKAY]
 └ Checking compute verbose quality (PalmLatent) logs... [OKAY]
Running QualityInterface::computeVerboseQuality() for UnknownLatent... [OKAY]
 ├ Merging verbose-UnknownLatent logs... [OKAY]
 └ Checking compute verbose quality (UnknownLatent) logs... [WARN]

================================================================================
| There were 4 failures to compute unified quality.                            |
| output/driver/verbose-UnknownLatent.log                                      |
================================================================================
Creating validation submission... (frifte_quality_validation_nullimpl_0001.tar.xz) [OKAY]

================================================================================
| Please review the marketing and CBEFF information compiled into your         |
| library to ensure correctness:                                               |
|                                                                              |
| CBEFF Product Owner = 0x000F                                                 |
| CBEFF Algorithm Identifier = 0xF1A7                                          |
| Marketing Name = NullImplementation Quality 1.0                              |
================================================================================

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ This script could not check online to ensure there are no updates            +
+ available. NIST requires that FRIF TE submissions always use the latest      +
+ version. Retrieve the latest version number by visiting the URL below and    +
+ be sure it matches this version: 202509240859.                               +
+                                                                              +
+ https://github.com/usnistgov/frifte/tree/main/quality/validation/VERSION     +
+                                                                              +
+ If these numbers don't match, visit our website to retrieve the latest       +
+ version.                                                                     +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

================================================================================
| You have successfully completed your part of FRIF TE Quality validation.     |
| Please sign and encrypt the file listed below (run './validate encrypt' for  |
| an example).                                                                 |
|                                                                              |
|                frifte_quality_validation_nullimpl_0001.tar.xz                |
|                                                                              |
| Please upload both frifte_quality_validation_nullimpl_0001.tar.xz.asc and    |
| your public key via https://pages.nist.gov/frifte/quality/upload             |
================================================================================
Completed: 28 Sep 2026 | 15:19:18 EDT (Runtime: 30s)
```
</details>

Submission Contents
-------------------

 * **canary.log:** MD5 checksums of randomly-generated images we provide as part
   of the validation fingerprint imagery. This helps us make sure that you are
   using the most recent version of FRIF TE Quality validation fingerprint
   imagery.
 * **compatibility.log:** Output from calling `Interface::getCompatibility()`.
 * **compile.log:** Output from compiling the validation executable and other
   information like checksums, versions, and library dependencies that may help
   us debug your submission if an error occurs.
 * **config/:** A copy of [config/].
 * **driver/:** Output from the validation executable:
   - **unified-*.log:** Output from calling
     `Interface::computeUnifiedQuality()`.
   - **verbose-*.log:** Output from calling
     `Interface::computeVerboseQuality()`.
 * **id.log:** Output from `Quality::getLibraryIdentifier()`.
 * **id_product.log:** Output from `Interface::getProductIdentifier()`.
 * **lib/:** A copy of [lib/].
 * **run-*.log:** The command used to launch the
   validation executable when generating `driver/*.log`.

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
 * Appropriately-named FRIF TE Quality core software library is present.
 * Software library links properly against the validation driver.
 * Crashes do not occur when handling various types of imagery, including
   - atypical resolutions;
   - atypical data sources (e.g., features-only, latent probe)
   - unknown metadata;
   - non-contact imagery;
   - blank or gradient patterns.

While the validation package tries to eliminate errors from the FRIF TE Quality
submission, it is not perfect, and there are still several ways in which the
package might approve you for submission that NIST may later reject.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please [open an issue]. Other questions may be addressed
to the [NIST FRIF TE team].

The FRIF TE team sends updates about the FRIF TEs to their mailing list. Enter
your e-mail address on the [mailing list site], or send a blank e-mail to
FRIFTE+subscribe@list.nist.gov to be automatically subscribed.

License
-------
The items in this repository are released in the public domain. See the
[LICENSE] for details.

[API]: https://pages.nist.gov/frifte/doc/api/quality.html
[Ubuntu Mirrors]: https://launchpad.net/ubuntu/+cdmirrors
[🇺🇸 USA]: https://mirror.math.princeton.edu/pub/ubuntu-iso/noble/ubuntu-24.04.3-live-server-amd64.iso
[🇪🇺 Europe]: http://mirror.init7.net/ubuntu-releases/noble/ubuntu-24.04.3-live-server-amd64.iso
[NIST Image Group]: https://nigos.nist.gov/evaluations/ubuntu-24.04.3-live-server-amd64.iso
[lib/]: https://github.com/usnistgov/frifte/tree/main/quality/validation/lib
[../libfrifte_quality/]: https://github.com/usnistgov/frifte/tree/main/quality/libfrifte_quality
[../../libfrifte/]: https://github.com/usnistgov/frifte/tree/main/libfrifte
[../../include/quality.h]: https://github.com/usnistgov/frifte/blob/main/include/frifte/quality.h
[bin/]: https://github.com/usnistgov/frifte/tree/main/quality/validation/bin
[config/]: https://github.com/usnistgov/frifte/tree/main/quality/validation/config
[README.md]: https://github.com/usnistgov/frifte/blob/main/quality/validation/README.md
[src/]: https://github.com/usnistgov/frifte/tree/main/quality/validation/src
[CHECKSUMS]: https://github.com/usnistgov/frifte/blob/main/quality/validation/CHECKSUMS
[VERSION]: https://github.com/usnistgov/frifte/tree/main/quality/validation/VERSION
[validate]: https://github.com/usnistgov/frifte/tree/main/quality/validation/validate
[NIST FRIF TE team]: mailto:frifte@nist.gov
[open an issue]: https://github.com/usnistgov/frifte/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/g/frif
[LICENSE]: https://github.com/usnistgov/frifte/blob/main/LICENSE.md
[test plan]: https://pages.nist.gov/frifte/doc/testplan/quality_testplan.pdf
[requests website]: https://nigos.nist.gov/datasets/frifte_quality_validation/request
[the FRIF TE upload form]: https://pages.nist.gov/frifte/quality/upload
[../../include/frifte]: https://github.com/usnistgov/frifte/tree/main/include/frifte
[../../include/frifte/quality.h]: https://github.com/usnistgov/frifte/blob/main/include/frifte/quality.h
