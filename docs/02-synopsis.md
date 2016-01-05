SYNOPSIS
========

`dit [.<project>] [key=value] [key+=value] [<command>] [args...]`

The command name is **dit** instead of **dit** as the first one is easier and
faster to type.

Broken down structure of the command-line:

```
  dit [.<project>] [key=value] [key+=value] [<command>] [args...]
   |         |     \                      /      |          |
command      |      ---------- -----------       |     sub-command
 name        |                |                  |      arguments
         optional             |            sub-command
       project-name           |                name
                        configuration
                          overrides
```

`<command>` in turn can be a composite of commands/aliases:

```
                 <command>. ... .<command>.<command>
                     /               |          \
              applied last           |         applied first
                              applied second
```
