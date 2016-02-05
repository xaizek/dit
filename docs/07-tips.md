TIPS
====

Install and use bash completion
-------------------------------

Bash completion script is installed in standard location and should work
automatically.  Lots of things are completed in a predictable and helpful
manner, so be sure to use the completion as it helps a great deal.

Create aliases for frequently used projects
-------------------------------------------

Example:

```bash
alias d.d='dit .dit'
alias d.v='dit .vifm'
```

Using dot as a separated might be better for using `Ctrl-W` with shell
configuration that stops at a dot.

Creating an alias for `dit` itself won't hurt either:

```bash
alias d='dit'
```

Note that this can break completion if there are not workarounds like the one
mentioned in http://superuser.com/a/437508.  Another option is to define
completion manually for the alias like this:

```bash
complete -F _dit d
```

This won't work right away with `d.d` and `d.v` above as they take project name
argument.
