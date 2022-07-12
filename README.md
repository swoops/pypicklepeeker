# pypicklepeeker
Py pickle peeker peeks a bushel of pickled python

Dissassembler for python pickles.

# Status

This is incomplete and likely broken. I wouldn't trust signedness and things
like that just yet. Nice to get a basic peek but needs more work.

Most opcodes are single bytes and take no parameters. So all opcodes have a
"UNHANDLED" handler until I actually check if the consumes a parameter from the
byte stream (as opposed to one of the stacks). If you see "UNHANDLED" in
output, and you will, it means the byte stream parsing might of gone off the
rails there if that opcode expected a parameter.

# Security

This code is just a dissassembler, it does NOT execute any of the pickle
opcodes. So even though it's C, it's a lot safer then unpickling with python
just to view an object.


# Contributing

If you want to, I won't stop you. :)

# example

```
> python3 -c 'import pickle;import sys;sys.stdout.buffer.write(pickle.dumps({"test":[1,2,"tree"],"bool":False}))'|./pypicklepeeker 
[0x000] PROTO 4
[0x002] FRAME 0x23
[0x00b] EMPTY_DICT
[0x00c] MEMOIZE
[0x00d] MARK
[0x00e] SHORT_BINUNICODE test
[0x014] MEMOIZE
[0x015] EMPTY_LIST
[0x016] MEMOIZE
[0x017] MARK
[0x018] BININT1 0x1
[0x01a] BININT1 0x2
[0x01c] SHORT_BINUNICODE tree
[0x022] MEMOIZE
[0x023] UNHANDLED -> APPENDS
[0x024] SHORT_BINUNICODE bool
[0x02a] MEMOIZE
[0x02b] UNHANDLED -> NEWFALSE
[0x02c] UNHANDLED -> SETITEMS
[0x02d] UNHANDLED -> STOP
```
