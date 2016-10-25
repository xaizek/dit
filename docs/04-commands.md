COMMANDS
========

Generic syntax elements
-----------------------

There are two big groups of commands:

1. Commands that operate on elements.
2. The rest of commands.

First group of commands usually accepts an item id and sometimes list of
fields to create or update.  Second group generally has less commonalities.

List of fields
--------------

The list specifies operations on entries of an item.  There are two kinds of
operations:

 - assigning
 - appending (added on a new line)

Where assigning to empty value means removal.

There are also two types of syntax which can be used simultaneously:

 - key=value
 - key: value

The first kind suits well for simple values without any special characters or
spaces, while the second one is great for all other cases.  Due to the way shell
works, on completion the second syntax is used (otherwise it's not convenient to
type the rest of the value after completion because of an extra space).

### List of fields: "key=value" and "key+=value" ###

One can use shell parsing capabilities to insert values with spaces or special
symbols:

```
status=done
status='in progress'
status=to\ be\ considered
title="Doesn't work with \$PATH = $PATH"
```

### List of fields: "key: long value" ###

This form is easier to type as there is no quotes around values.

```
status: done
status: in progress
status: to be considered
title: Doesn\'t work with \$PATH = "$PATH"
```

During parsing contiguous spaces are contracted into single one.

### List of fields: How ambiguity is resolved ###

If an argument includes `=` character and what precedes it is a valid key name,
then it's assumed to be "key=value" form of argument.  Otherwise it must end
with a colon to be key name or be preceded by such argument.

Such rules help with parsing the following strings as values:

```
2+2=4
http://example.com/script?param=value
```

### List of fields: Requesting spawning external editor ###

When assigning a value to a key, it's possible to specify special value of
**-** (single dash character), which instructs the application to spawn editor
to update corresponding field.

On appending editor contains previous value of the field is used as starting
value.  Once editor is closed, contents of the file is read.

Example:

```bash
dit set abc assign=- append+=- assign_another_field: -
```

List of conditions
------------------

This is list of arguments where each argument is a comparison expression of the
form:

```
key op value
```

*key* and *value* are two parts of an entry. *op* is one of:

 * **==** -- *key* is equal to the *value*
 * **!=** -- *key* is not equal to the *value*
 * **/** or **=/** -- *key* contains *value* (case is ignored)
 * **#** or **!/** -- *key* doesn't contain *value* (case is ignored)

Extra spaces are allowed, but don't forget to escape them (with \\ or quotes).

Command composition
-------------------

When given command in the form of `<command>. ... .<command>.<command>` here's
what happens.

Components are processed from right to left with aliases being expanded
(commands are just expanded to themselve).  Each alias can be viewed as three
components: configuration overrides, command name and arguments.  For each alias
its components are mixed into current command-line as follows:

 1. overrides = overrides + alias overrides          (appended)
 2. Command name from an alias is new command name.
 3. arguments = expanded alias + extra arguments     (prepended)

`expanded alias` is created by applying alias to current argument list.
`extra arguments` are all arguments unused by the alias.

One of the commands can be empty (as in `.cmd`, `cmd1..cmd2` or `cmd.`), in
which case it designates default command line, which is expanded at that point
of processing.  Note that leading dot can be taken as indication of project
name, and thus should be used with project specified explicitly.  As a special
case single dot as a command (`.`) is expanded into default command line (this
is the same as empty command name (`''`), but easier to type).

For example, the following aliases:

```
alias.recent = ui.ls.sort=_changed,title,_id
alias.dates = ui.ls.fmt+=,_created,_changed
alias.standout = ls status!=done
```

combined as `recent.standout` or `standout.recent` (doesn't matter in this case)
yield the following command-line:

```
ui.ls.sort=_changed,title,_id alias.standout = ls status!=done
```

`recent.dates.ls` produces:

```
ui.ls.sort=_changed,title,_id ui.ls.fmt+=,_created,_changed ls
```
