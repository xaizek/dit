CONFIGURATION
=============

Hierarchy of configurations
---------------------------

The lookup is performed from left to right according to the following diagram:

```
Overrides --> Project --> Global --> Application Default
```

Thus options given on the command-line are the most prioritized, then
project-specific settings are queried, then global configuration and defaults
are used if nothing specifies the value.

Accessing settings
------------------

**config** command is there to view and change settings.  By default
project-specific settings are used, but **--global** option switches to global
configuration.

Setting names
-------------

Settings starting with **!** are reserved for internal purposes and can't be
accessed via **config**.

Setting key is a dot-separated sequence of section names.  It can't be empty.

Configuration overrides
-----------------------

These are command-line arguments between project name and command (all of these
are optional, so they might go first and not be followed by anything) of the
form:

 - `key=value` -- sets new value
 - `key+=value` -- appends to existing value

Such settings are temporary and not saved anywhere, project-specific or global
configurations should be used for persistent settings.  These are good for tests
and use with aliases.

Aliases
-------

Aliases can only be set in global configuration, otherwise they are ignored.

Alias name can match name of a builtin command.

Aliases must be defined in *alias* section, e.g. *alias.standout*.

Alias can consume arguments, their placeholders have the form: **{<num>}**,
numbering starts with 1.  Unused arguments are appended.

Example:

```
dit config --global alias.take = set ${1} status='in progress'
dit take id comment+='Done, finally.'
# equivalent to: dit set id status='in progress' comment+='Done, finally.'
```

Removing values
---------------

Removal is performed by reseting key to empty value, which will enable use of
application default value or value from the previous level if such values exist.

Example:

```bash
dit config ui.lf.fmt=
```

List of all settings
--------------------

**core.defcmd** (global) (default: `ls`) --
names command to be used on invocation without arguments.

**core.defprj** (global) (no default) --
names project to use if none was specified.

**prj.descr** (no default) --
project description for **projects** command.

**ui.ls.fmt** (default: `_id,title`) --
column specification for **ls** command.

**ui.ls.sort** (default: `title,_id`) --
item sorting specification for **ls** command.

**ui.ls.color** (default: `fg-cyan inv bold !heading`) --
item colorization rules for **ls** command.

**ui.show.order** (default: `title`) --
entries ordering specification for **show** command.

### ui.ls.fmt syntax ###

Comma-separated list of keys, which define columns of table printed out by
**ls** command.

### ui.ls.sort syntax ###

Comma-separated list of keys.  First key is the primary one, its subgroups are
sorted by the second key and so on (i.e., items are sorted by keys in right to
left order preserving relative ordering from previous sorts).

### ui.ls.color syntax ###

Semicolon-separated list of colorization rules.  Each rule has the following
structure:

```
attributes conditions
```

Attributes can include:

 * **bold**, **inv**, **def**;
 * **fg-black**, **fg-red**, **fg-green**, **fg-yellow**, **fg-blue**,
   **fg-magenta**, **fg-cyan**, **fg-white**;
 * **bg-black**, **bg-red**, **bg-green**, **bg-yellow**, **bg-blue**,
   **bg-magenta**, **bg-cyan**, **bg-white**.

Conditions are either of:

 * **!heading** -- header of the table;
 * any condition as in list of conditions.

### ui.show.order syntax ###

Comma-separated list of keys.  Keys mentioned in this option are displayed above
any other keys and in this order, the rest keys of an item follow in
alphabetical order.
