### Configuration defaults ###

#### For project ####

Using `Config::get(name, defaultValue)`.

#### For global configuration ####

Using parent configuration of global configuration filled with default values
during initialization of main application class.

### Object initialization ###

For performance reasons main classes like `Config`, `Project`, `Storage`, `Item`
all initialize themselves lazily (with help of `StorageBacked` class).  To
facilitate this each of them initializes crucial things in constructors, but
does actual load in private `load()` method and public API should call
`ensureLoaded()` if they rely on lazy-loadable data.

### Commands ###

#### Execution and completion ####

There are two virtual methods for each of the action: one for global operation
and one for project specific action.  First one gets application reference while
the second one gets project reference.  Invocation order is: global then project
specific.  One can do all the work at project specific level (if it makes sense)
by saving data passed into global handler for future use.

Completion is a first-class operation and is handled on many levels of code.

#### Registration ####

This is done automatically by `AutoRegisteredCommand` class.  Commands just need
to derive from it (CRTP is involved).

### Interaction with tests ###

Tests mostly use public API, but sometimes it's not enough, in which case
Passkey idiom is used to provide dedicated testing API.  Passkeys allow
accessing particular methods for `Tests` class, which provides a set of static
methods for use in test cases.
