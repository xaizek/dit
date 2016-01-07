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

* `coverage`
Builds coverage in `coverage/` putting report in `coverage/data/`.

* `show-coverage`
Same as `coverage` plus invokes `$BROWSER` on index file of the coverage report.

* `check`
Builds and runs tests.  Combine with `debug` or `release` to build and run tests
in that configuration.

* `clean`
Removes build artifacts for all configurations.

#### Documentation ####

Building a man-page requires `pandoc` to be installed.
