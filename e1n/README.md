Exemplar One-to-Many
--------------------
This directory contains source code useful to participate in FRIF TE Exemplar One-to-Many (Exemplar 1:N or E1N, for short). The [E1N API] itself, [e1n.h], is in the global [include] directory.

 * [libfrif_e1n]:
     - Implementation of `libfrif_e1n.so`---routines required by the [E1N API].
 * [nullimpl]:
     - Stub implementation of the [E1N API].
 * [validation]
     - Code to test and package [E1N API] implementations for submission to NIST for evaluation.

Communication
-------------
If you found a bug and can provide steps to reliably reproduce it, or if you
have a feature request, please [open an issue]. Other questions may be addressed
to the [NIST FRIF team].

The FRIF team sends updates about the FRIF tests to their mailing list. Enter
your e-mail address on the [mailing list site], or send a blank e-mail to
FRIF+subscribe@list.nist.gov to be automatically subscribed.

License
-------
The items in this repository are released in the public domain. See the
[LICENSE] for details.


[E1N API]: https://pages.nist.gov/frif/doc/api/e1n.html
[e1n.h]: https://github.com/usnistgov/frif/blob/main/include/frif/e1n.h
[libfrif_e1n]: https://github.com/usnistgov/frif/blob/main/e1n/libfrif_e1n
[nullimpl]: https://github.com/usnistgov/frif/blob/main/e1n/nullimpl
[validation]: https://github.com/usnistgov/frif/blob/main/e1n/validation
[include]: https://github.com/usnistgov/frif/blob/main/include
[open an issue]: https://github.com/usnistgov/frif/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/frif/join
[LICENSE]: https://github.com/usnistgov/frif/blob/main/LICENSE.md
[NIST FRIF team]: mailto:frif@nist.gov
