EXAMPLE
=======

Below is an example sequence of commands that demonstrates basic usage.

Assuming absent configuration, there isn't much one can do:

```bash
$ dit
No project specified
$ dit projects
```

Create first project:

```bash
$ dit new test
$ dit
No project specified
$ dit projects
 test
$ dit .test
ID  TITLE
```

Setup the `test` project to be the default one to omit `.test` part:

```bash
$ dit config --global core.defprj: test
$ dit
ID  TITLE
```

Now add simple task:

```bash
$ dit add title: Explore dit.
Created item: fqH
$ dit
ID   TITLE
fqH  Explore dit.
```

Modify its field:

```bash
$ dit set fqH status: in progress
```

It's not visible via `ls` (the default subcommand), but it's there:

```bash
$ dit
ID   TITLE
fqH  Explore dit.
$ dit show fqH
title: Explore dit.
status: in progress
```

To display it via `ls`, configuration must be altered a bit:

```bash
$ dit config ui.ls.fmt: _id,title,status
$ dit
ID   TITLE         STATUS
fqH  Explore dit.  in progress
```

One might want to ease taking tasks and marking them as done later, this is
where aliases can be used:

```bash
$ dit config --global alias.done='set ${1} status=done'
$ dit config --global alias.take='set ${1} status="in progress"'
$ dit done fqH
$ dit
ID   TITLE         STATUS
fqH  Explore dit.  done
```

Usually, there is no need to see tasks that are already done and `ls` command
drops them from the output if asked:

```bash
$ dit ls status!=done
ID  TITLE  STATUS
```

Not very easy to type, so let's make an alias and set it as new default command:

```bash
$ dit config --global alias.standout='ls status!=done'
$ dit config --global core.defcmd: standout
$ dit
ID  TITLE  STATUS
$ dit ls
ID   TITLE         STATUS
fqH  Explore dit.  done
```

It's sometimes handy to add additional columns to the table, aliases can help
here as well:

```bash
$ dit config --global alias.dates='ui.ls.fmt+=,_created,_changed'
$ dit ls.dates
ID   TITLE         STATUS  CREATED              CHANGED
fqH  Explore dit.  done    2016-02-05 12:31:25  2016-02-05 13:07:51
```

If at some later moment one wants to view history of a task, use `log`:

```bash
$ dit log fqH
title created: Explore dit.
status created: in progress
status changed:
-in progress
+done
```

And so on...

Combination of aliases and key-value pairs allow for implementation of wide
range of possible workflows, one only needs to adjust them accordingly.
