# Coredumps

Core dumps should be short enough to be written to the stderr in case of crash, with enough information to be able
to recreate the stacktraces.
In addition, it might be appended with extra information to have extra level of introspection.

Here is the format of a given core dump:
Git hash
Bazel argument to build the binary
stack trace addresses
register info

With this and the help of tool, we should be able to rebuild the binary used and exrtact a human readable stack trace

- more information if needed.

Some information about core dump file format:

- https://www.gabriel.urdhr.fr/2015/05/29/core-file/
- http://uhlo.blogspot.com/2012/05/brief-look-into-core-dumps.html
- https://github.com/Percona-Lab/coredumper

- https://github.com/efiop/criu-coredump/blob/master/criu_coredump/core_dump.py
- https://stackoverflow.com/questions/17972945/core-dump-note-section
- https://stackoverflow.com/questions/26641418/creating-coredump-for-arm-based-embedded-system

The most relevant:

- https://github.com/checkpoint-restore/criu/tree/criu-dev/coredump/criu_coredump

To create a elf file, we can use: https://github.com/serge1/ELFIO
