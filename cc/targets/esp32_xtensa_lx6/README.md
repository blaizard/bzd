# Debug

To read the backtrace after a crash for example:

```
abort() was called at PC 0x4012f72f on core 0


Backtrace: 0x400816c6:0x3ffbdb30 0x40085835:0x3ffbdb50 0x400896d6:0x3ffbdb70 0x4012f72f:0x3ffbdbe0 0x4011957e:0x3ffbdc30 0x401585df:0x3ffbde50 0x400e9b11:0x3ffbde80 0x400f8379:0x3ffbdeb0 0x400f1726:0x3ffbdf20 0x400ec895:0x3ffbdf70 0x400ed33a:0x3ffbdff0 0x401182bd:0x3ffbe1a0 0x400d69ab:0x3ffbe1d0 0x4015dffe:0x3ffbe1f0
```

Each PC pointer can be translated to code line with the following command:

```bash
addr2line -f -e bazel-bin/apps/jardinier2000/jardinier2000.binary 0x400816c6
```
