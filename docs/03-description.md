DESCRIPTION
===========

Overview
--------

The overall hierarchical structure of data is as follows:

```
+-------------------+         +-------------+
|   Project A       |         |  Project X  |
|                   |    .    |             |
| Item a    Item b  |         |  Item a     |
| ------    ------  |    .    |  ------     |
| entry1    entry1  |         |  entry1     |
| entry2    entry2  |    .    |             |
|           entry3  |         |             |
|                   |         |             |
+-------------------+         +-------------+
          |                           |
    +-----------+               +-----------+
    | Project A |-------+-------| Project B |
    |  Config   |       |       |  Config   |
    +-----------+       |       +-----------+
                        |
                        v
               +-----------------+
               |  Global config  |
               +-----------------+

```

Projects
--------

Projects define storage namespace for items and configuration settings.
Project-specific settings can override almost any of the global ones.

Project manages distribution of unique identifies among items constituting it.

IDs
---

Identifies are three character strings picked in semi-randomised way that
guarantees that they won't repeat.  The space is picked at the moment of project
creation.  Each character can be one of 26 lower case and 26 upper case letters
of Latin alphabet.  IDs start as at the length of three and then automatically
extended.

Items
-----

Each item in a project is uniquely identified by its symbolic ID.  Items consist
of entries, which are key-value pairs.  Removing of an entry is performed by
assigning empty value to it.

Entries
-------

Users' key names start with one of `[a-zA-Z]` characters and are followed by any
of `[-a-zA-Z_0-9]`.  Builtin keys start with `_`.

Currently available builtin keys:

 - `_id` -- identifier of the item;
 - `_created` -- creation date and time of the item;
 - `_changed` -- date and time the item was last changed.

Values can contain arbitrary strings, which might include newline characters.

Changes
-------

When a change is made to an entry, instead of actually overwriting an old value
with a new one, new one just hides old value keeping whole history of item
changes available for the future.
