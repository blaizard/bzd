# Coredumps

Core dumps should be short enough to be written to the stderr in case of crash, with enough information to be able
to recreate teh stacktraces.
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

To create a elf file, we can use: https://github.com/serge1/ELFIO
