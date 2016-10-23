SYNOPSIS
========

`dit [.<project>] [key=value] [key+=value] [<command>] [args...]`

Broken down structure of the command-line:

```
  dit [.<project>] [key=value] [key+=value] [<command>] [args...]
   |         |     \                      /      |          |
command      |      ---------- -----------       |     sub-command
 name        |                |                  |      arguments
         optional             |            sub-command
       project-name           |                name
       (. == defprj)    configuration
                          overrides
```

`<command>` in turn can be a composite of commands/aliases:

```
                 <command>. ... .<command>.<command>
                     /               |          \
              applied last           |         applied first
                              applied second
```
