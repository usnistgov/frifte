FRIF Null Implementation
========================

This directory contains example code that could be used as a starting point for
a [E1N API] implementation.

Building
--------
Use the included `CMakeLists.txt` to build out of source. This will build
`libfrif_e1n_nullimpl_0001.so` and its dependencies, [`libfrif`] and
[`libfrif_e1n`].

```sh
mkdir build
cd build
cmake ..
make
```

The name and version number for the library to be built are extracted from
within the C++ source.

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

[`libfrif`]: https://github.com/usnistgov/FRIF/blob/main/libfrif
[`libfrif_e1n`]: https://github.com/usnistgov/FRIF/blob/main/e1n/libfrif_e1n
[NIST FRIF team]: mailto:FRIF@nist.gov
[open an issue]: https://github.com/usnistgov/FRIF/issues
[mailing list site]: https://groups.google.com/a/list.nist.gov/forum/#!forum/FRIF/join
[LICENSE]: https://github.com/usnistgov/FRIF/blob/main/LICENSE.md
[E1N API]: https://pages.nist.gov/frif/doc/api/e1n.html
