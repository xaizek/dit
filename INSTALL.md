### Building ###

#### Defaults ####

By default debug version is build right in the source tree.

#### Targets ####

* `debug`
Builds debug version in `debug/`.

* `release`
Builds release version in `release/`.

* `man`
Builds manual page in `<out>/docs/dit.1`, requires `pandoc`.

* `with-coverage`
Builds with coverage in current directory and generates no report.

* `coverage`
Builds coverage in `coverage/` putting report in `coverage/data/`.  Requires
`lcov` to be installed.

* `show-coverage`
Same as `coverage` plus invokes `$BROWSER` on index file of the coverage report.
Requires `lcov` to be installed.

* `upload-coverage`
Gathers coverage data and sends it to coveralls.  Requires `cpp-coveralls` to be
installed.  The assumed usege is like this:

    make clean
    make with-coverage
    make upload-coverage TOKEN=<token-value>

* `check`
Builds and runs tests.  Combine with `debug` or `release` to build and run tests
in that configuration.

* `clean`
Removes build artifacts for all configurations.

* `install`
Builds the application in release mode (`release` target) and installs it.
`DESTDIR` can be set to point to root of the installation directory.

* `install-docs`
Builds man page (`man` target) and installs it.
`DESTDIR` can be set to point to root of the installation directory.

* `uninstall`
Removes executable and man page.

#### Documentation ####

Building a man-page requires `pandoc` to be installed.

### Installation ###

A regular installation under `/usr/bin` can be done like this:

```
make install install-docs
```

Installation into custom directory (e.g. to make a package):

```
make DESTDIR=$PWD/build install install-docs
```

### Uninstallation ###

Similar to installation, but using `uninstall` target:

```
make uninstall
```

Or for custom location:

```
make DESTDIR=$PWD/build uninstall
```
