LIST OF COMMANDS
================

add
---

Adds new item.

**Usage: add \<list of fields\>**

Creates new item filling it with specified entries.

check
-----

Verifies project state.

**Usage: check**

Checks that storage files and their content are meaningful.  Prints out errors
if something is wrong and exits with non-zero exit code.

complete
--------

Command-line completion helper for shells.

**Usage: complete \<regular args\>**

Takes normal command-line with trailing *::cursor::* designating cursor
position.  The marker is needed to complete empty argument, which otherwise
might be dropped by the shell on invocation.

config
------

Displays/updates configuration.

**Usage: config [--help|-h] [--global|-g] \<list of fields\>**

**--help (-h)** causes option summary to be printed.

**--global (-g)** switches operations to act on global configuration, whereas by
default project-specific configuration is processed.

When invoked without arguments, all values are displayed.

When invoked with at least one argument:

 - Keys without values are displayed.
 - Keys with values are updated.  Setting is removed on assigning it empty
   value.

Special value **-** can be used to request spawning external editor.

export
------

Invokes external script passing item data via argument list.

**Usage: export (-|cmd) \<list of conditions\>**

Invokes **cmd key1=value1 key2=value2** for each item that matches given list
of conditions or prints out items to standard output with **key=value** fields
terminated by null character and each item also finished by null character.
Builtin key `_id` is also printed.

help
----

Provides help information.

**Usage: help [command]**

Without arguments displays summary of available commands.

With argument displays summary on that command.

log
---

Displays item changes.

**Usage: log [--help|-h] [--timestamps|-t] \<item id\> [key...]**

**--help (-h)** causes option summary to be printed.

**--timestamps (-t)** adds timestamp to each change printed.

Displays information about item changes (from oldest to newest) either for all
fields (if only item id is specified) or just for the specified ones.

ls
--

Lists items.

**Usage: ls \<list of conditions\>**

Print table of items that match the list of conditions.

Affected by: **ui.ls.fmt**, **ui.ls.sort**, **ui.ls.color**.

new
---

Create new project.

**Usage: new \<project name\>**

Initializes new project.

projects
--------

Lists projects.

**Usage: projects**

Lists projects along with their descriptions (**prj.descr**).  Picked project is
marked with a star (\*).

rename
------

Rename project.

**Usage: rename \<old name\> \<new name\>**

Renames existing project.

set
---

Changes items.

**Usage: set \<item id\> \<list of fields\>**

Updates entries of existing item.

show
----

Displays items.

**Usage: show \<item id\> [key...]**

Prints entries of specified item, either all fields (if only item id is
specified) or just for the specified ones.

Affected by: **ui.show.order**.
