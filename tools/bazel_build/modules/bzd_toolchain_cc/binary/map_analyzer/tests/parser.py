import sys
import unittest
import typing
import pathlib

from binary.map_analyzer.parser.gcc import ParserGcc
from binary.map_analyzer.parser.clang import ParserClang


class TestRun(unittest.TestCase):
	pathGccEsp32XtensaLx6: typing.Optional[pathlib.Path] = None
	pathGccX86: typing.Optional[pathlib.Path] = None
	pathClang: typing.Optional[pathlib.Path] = None

	def testParserGccEsp32XtensaLx6(self) -> None:
		self.maxDiff = None

		assert self.pathGccEsp32XtensaLx6
		parser = ParserGcc(self.pathGccEsp32XtensaLx6)
		result = parser.parse()
		self.assertTrue(result)

		parser.filter(".debug_*")
		parser.filter(".comment")
		parser.filter(".symtab")
		parser.filter(".xt.lit*")
		parser.filter(".xt.prop*")

		self.assertDictEqual(
		    parser.getBySections(),
		    {
		        ".dram0.bss": {
		            "address": 1073422352,
		            "size": 29664
		        },
		        ".dram0.data": {
		            "address": 1073414144,
		            "size": 8208
		        },
		        ".flash.rodata": {
		            "address": 1061158944,
		            "size": 89472
		        },
		        ".flash.text": {
		            "address": 1074593816,
		            "size": 230350
		        },
		        ".iram0.text": {
		            "address": 1074267136,
		            "size": 36087
		        },
		        ".iram0.vectors": {
		            "address": 1074266112,
		            "size": 1024
		        },
		        ".xtensa.info": {
		            "address": 0,
		            "size": 56
		        },
		    },
		)
		self.assertDictEqual(
		    parser.getByUnits(),
		    {
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libfreertos.a": {
		            ".iram0.vectors": 425,
		            ".iram0.text": 12004,
		            ".dram0.data": 4140,
		            ".dram0.bss": 776,
		            ".flash.rodata": 1060,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libapp_update.a": {
		            ".iram0.text": 109,
		            ".dram0.bss": 4,
		            ".flash.rodata": 256,
		            ".flash.text": 103,
		            ".xtensa.info": 56,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libesp32.a": {
		            ".iram0.text": 6075,
		            ".dram0.data": 1954,
		            ".dram0.bss": 106,
		            ".flash.rodata": 601,
		            ".flash.text": 4336,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libesp_common.a": {
		            ".iram0.text": 441,
		            ".dram0.data": 16,
		            ".dram0.bss": 2240,
		            ".flash.rodata": 20,
		            ".flash.text": 975,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libheap.a": {
		            ".iram0.text": 2364,
		            ".dram0.data": 4,
		            ".dram0.bss": 4,
		            ".flash.text": 647,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libnewlib.a": {
		            ".iram0.text": 836,
		            ".dram0.data": 152,
		            ".dram0.bss": 272,
		            ".flash.rodata": 89,
		            ".flash.text": 869,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libpthread.a": {
		            ".iram0.text": 211,
		            ".dram0.data": 16,
		            ".dram0.bss": 12,
		            ".flash.text": 617,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libsoc.a": {
		            ".iram0.text": 6522,
		            ".dram0.data": 24,
		            ".dram0.bss": 4,
		            ".flash.rodata": 3309,
		            ".flash.text": 707,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libspi_flash.a": {
		            ".iram0.text": 4460,
		            ".dram0.data": 520,
		            ".dram0.bss": 294,
		            ".flash.text": 1019,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libxtensa.a": {
		            ".iram0.text": 196
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libbootloader_support.a": {
		            ".iram0.text": 1020,
		            ".flash.text": 495,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libhal.a": {
		            ".iram0.text": 447,
		            ".flash.rodata": 32,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libesp_ringbuf.a": {
		            ".iram0.text": 858,
		            ".flash.rodata": 154,
		        },
		        "/home/dummy1/.cache/bazel/_bazel_blaise/487b37b7551c3a6035cab0c6471a2d4b/external/esp32_xtensa_lx6_gcc_8.2.0/lib/gcc/xtensa-esp32-elf/8.2.0/no-rtti/libgcc.a":
		            {
		                ".iram0.text": 104,
		                ".dram0.data": 4,
		                ".dram0.bss": 12,
		                ".flash.rodata": 872,
		                ".flash.text": 5270,
		            },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libcxx.a": {
		            ".dram0.data": 8,
		            ".dram0.bss": 16,
		            ".flash.text": 525,
		        },
		        "external/esp32_xtensa_lx6_gcc_8.2.0/xtensa-esp32-elf/lib/libc.a": {
		            ".dram0.data": 364,
		            ".dram0.bss": 30,
		            ".flash.rodata": 4541,
		            ".flash.text": 79615,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libvfs.a": {
		            ".dram0.data": 308,
		            ".dram0.bss": 48,
		            ".flash.rodata": 162,
		            ".flash.text": 5376,
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/example/liblib.a": {
		            ".dram0.data": 188,
		            ".dram0.bss": 20099,
		            ".flash.rodata": 1167,
		            ".flash.text": 2765,
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/core/libscheduler.a": {
		            ".dram0.data": 96,
		            ".dram0.bss": 1,
		            ".flash.rodata": 571,
		            ".flash.text": 608,
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/core/libassert_minimal.a": {
		            ".dram0.data": 104,
		            ".dram0.bss": 13,
		            ".flash.rodata": 353,
		            ".flash.text": 1055,
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/out/std/libstd.lo": {
		            ".dram0.data": 40,
		            ".dram0.bss": 1,
		            ".flash.rodata": 101,
		            ".flash.text": 75,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libdriver.a": {
		            ".dram0.data": 84,
		            ".dram0.bss": 20,
		            ".flash.rodata": 280,
		            ".flash.text": 2919,
		        },
		        "external/esp32_xtensa_lx6_gcc_8.2.0/xtensa-esp32-elf/lib/libstdc++.a": {
		            ".dram0.data": 153,
		            ".dram0.bss": 5648,
		            ".flash.rodata": 76810,
		            ".flash.text": 120088,
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/stack/stub/libstub.a": {
		            ".flash.rodata": 1,
		            ".flash.text": 10,
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/libplatform.a": {
		            ".flash.rodata": 60,
		            ".flash.text": 117,
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libmain.a": {
		            ".flash.text": 8
		        },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/clock/esp32_xtensa_lx6/libesp32_xtensa_lx6.lo":
		            {
		                ".flash.text": 26
		            },
		        "bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/panic/exit/libexit.lo": {
		            ".flash.text": 16
		        },
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libefuse.a": {},
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/liblog.a": {},
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libmbedtls.a": {},
		        "external/esp32_xtensa_lx6_sdk_4.0/lib/libwpa_supplicant.a": {},
		    },
		)

	def testParserGccX86(self) -> None:
		assert self.pathGccX86
		parser = ParserGcc(self.pathGccX86)
		result = parser.parse()
		self.assertTrue(result)

		parser.filter(".debug_*")
		parser.filter(".comment")
		parser.filter(".symtab")

		self.assertDictEqual(
		    parser.getBySections(),
		    {
		        ".interp": {
		            "address": 624,
		            "size": 28
		        },
		        ".note.ABI-tag": {
		            "address": 652,
		            "size": 32
		        },
		        ".note.gnu.build-id": {
		            "address": 684,
		            "size": 36
		        },
		        ".dynsym": {
		            "address": 720,
		            "size": 11472
		        },
		        ".dynstr": {
		            "address": 12192,
		            "size": 32813
		        },
		        ".gnu.hash": {
		            "address": 45008,
		            "size": 3376
		        },
		        ".gnu.version": {
		            "address": 48384,
		            "size": 956
		        },
		        ".gnu.version_r": {
		            "address": 49340,
		            "size": 192
		        },
		        ".rela.dyn": {
		            "address": 49536,
		            "size": 1560
		        },
		        ".rela.plt": {
		            "address": 51096,
		            "size": 408
		        },
		        ".init": {
		            "address": 51504,
		            "size": 23
		        },
		        ".plt": {
		            "address": 51536,
		            "size": 288
		        },
		        ".text": {
		            "address": 51824,
		            "size": 24898
		        },
		        ".fini": {
		            "address": 76724,
		            "size": 9
		        },
		        ".rodata": {
		            "address": 76736,
		            "size": 3162
		        },
		        ".gcc_except_table": {
		            "address": 79898,
		            "size": 111
		        },
		        ".eh_frame": {
		            "address": 80016,
		            "size": 12428
		        },
		        ".eh_frame_hdr": {
		            "address": 92444,
		            "size": 3004
		        },
		        ".tbss": {
		            "address": 100904,
		            "size": 104
		        },
		        ".data.rel.ro.local": {
		            "address": 100904,
		            "size": 24
		        },
		        ".fini_array": {
		            "address": 100928,
		            "size": 8
		        },
		        ".init_array": {
		            "address": 100936,
		            "size": 40
		        },
		        ".data.rel.ro": {
		            "address": 100976,
		            "size": 448
		        },
		        ".dynamic": {
		            "address": 101424,
		            "size": 752
		        },
		        ".got": {
		            "address": 102176,
		            "size": 64
		        },
		        ".got.plt": {
		            "address": 102240,
		            "size": 160
		        },
		        ".data": {
		            "address": 102400,
		            "size": 32
		        },
		        ".bss": {
		            "address": 102432,
		            "size": 20273
		        },
		        ".note.gnu.gold-version": {
		            "address": 0,
		            "size": 28
		        },
		        ".strtab": {
		            "address": 0,
		            "size": 45531
		        },
		        ".shstrtab": {
		            "address": 0,
		            "size": 432
		        },
		    },
		)
		self.assertDictEqual(
		    parser.getByUnits(),
		    {
		        "/usr/lib/x86_64-linux-gnu/Scrt1.o": {
		            ".note.ABI-tag": 32,
		            ".text": 43,
		            ".data": 4,
		        },
		        "/usr/lib/x86_64-linux-gnu/crti.o": {
		            ".init": 18,
		            ".fini": 4
		        },
		        "/usr/lib/x86_64-linux-gnu/crtn.o": {
		            ".init": 5,
		            ".fini": 5
		        },
		        "/usr/lib/gcc/x86_64-linux-gnu/7/crtbeginS.o": {
		            ".text": 218,
		            ".fini_array": 8,
		            ".init_array": 8,
		            ".data": 8,
		            ".bss": 1,
		        },
		        "bazel-out/k8-fastbuild/bin/example/liblib.a": {
		            ".text": 16299,
		            ".rodata": 1518,
		            ".gcc_except_table": 91,
		            ".data.rel.ro.local": 8,
		            ".init_array": 8,
		            ".data.rel.ro": 184,
		            ".data": 8,
		            ".bss": 20216,
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/core/libscheduler.a": {
		            ".text": 1334,
		            ".rodata": 1043,
		            ".tbss": 104,
		            ".data.rel.ro.local": 8,
		            ".init_array": 8,
		            ".bss": 1,
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/core/libassert_minimal.a": {
		            ".text": 5307,
		            ".rodata": 424,
		            ".gcc_except_table": 8,
		            ".data.rel.ro.local": 8,
		            ".init_array": 8,
		            ".data.rel.ro": 192,
		            ".bss": 17,
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/platform/libplatform.a": {
		            ".text": 303,
		            ".gcc_except_table": 4,
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/platform/clock/std/libstd.lo": {
		            ".text": 581,
		            ".gcc_except_table": 4,
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/platform/out/std/libstd.lo": {
		            ".text": 264,
		            ".rodata": 89,
		            ".gcc_except_table": 4,
		            ".init_array": 8,
		            ".data.rel.ro": 72,
		            ".data": 8,
		            ".bss": 1,
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/platform/panic/exit/libexit.lo": {
		            ".text": 14
		        },
		        "bazel-out/k8-fastbuild/bin/cc/bzd/platform/stack/x86_64_system_v/libx86_64_system_v.a": {
		            ".text": 265,
		            ".rodata": 1,
		        },
		        "/usr/lib/x86_64-linux-gnu/libc_nonshared.a": {
		            ".text": 114
		        },
		        "/usr/lib/gcc/x86_64-linux-gnu/7/crtendS.o": {
		            ".eh_frame": 4
		        },
		    },
		)

	def testParserClang(self) -> None:
		self.maxDiff = None

		# Working case
		assert self.pathClang
		parser = ParserClang(self.pathClang)
		result = parser.parse()
		self.assertTrue(result)

		parser.filter(".debug_*")
		parser.filter(".comment")
		parser.filter(".symtab")

		self.assertDictEqual(
		    parser.getBySections(),
		    {
		        ".interp": {
		            "size": 28,
		            "address": 2097888
		        },
		        ".note.ABI-tag": {
		            "size": 32,
		            "address": 2097916
		        },
		        ".note.gnu.build-id": {
		            "size": 32,
		            "address": 2097948
		        },
		        ".dynsym": {
		            "size": 2160,
		            "address": 2097984
		        },
		        ".gnu.version": {
		            "size": 180,
		            "address": 2100144
		        },
		        ".gnu.version_r": {
		            "size": 112,
		            "address": 2100324
		        },
		        ".gnu.hash": {
		            "size": 416,
		            "address": 2100440
		        },
		        ".dynstr": {
		            "size": 5330,
		            "address": 2100856
		        },
		        ".rela.dyn": {
		            "size": 264,
		            "address": 2106192
		        },
		        ".rela.plt": {
		            "size": 576,
		            "address": 2106456
		        },
		        ".rodata": {
		            "size": 982,
		            "address": 2107040
		        },
		        ".gcc_except_table": {
		            "size": 408,
		            "address": 2108024
		        },
		        ".eh_frame_hdr": {
		            "size": 316,
		            "address": 2108432
		        },
		        ".eh_frame": {
		            "size": 1492,
		            "address": 2108752
		        },
		        ".text": {
		            "size": 8070,
		            "address": 2113536
		        },
		        ".init": {
		            "size": 23,
		            "address": 2121608
		        },
		        ".fini": {
		            "size": 9,
		            "address": 2121632
		        },
		        ".plt": {
		            "size": 400,
		            "address": 2121648
		        },
		        ".tbss": {
		            "size": 104,
		            "address": 2122048
		        },
		        ".fini_array": {
		            "size": 8,
		            "address": 2125824
		        },
		        ".init_array": {
		            "size": 8,
		            "address": 2125832
		        },
		        ".data.rel.ro": {
		            "size": 216,
		            "address": 2125840
		        },
		        ".dynamic": {
		            "size": 688,
		            "address": 2126056
		        },
		        ".got": {
		            "size": 72,
		            "address": 2126744
		        },
		        ".got.plt": {
		            "size": 216,
		            "address": 2126816
		        },
		        ".data": {
		            "size": 20072,
		            "address": 2129920
		        },
		        ".bss": {
		            "size": 104,
		            "address": 2149992
		        },
		        ".shstrtab": {
		            "size": 433,
		            "address": 0
		        },
		        ".strtab": {
		            "size": 6322,
		            "address": 0
		        },
		    },
		)
		self.assertDictEqual(
		    parser.getByUnits(),
		    {
		        "<internal>": {
		            ".interp": 28,
		            ".note.gnu.build-id": 32,
		            ".dynsym": 2160,
		            ".gnu.version": 180,
		            ".gnu.version_r": 112,
		            ".gnu.hash": 416,
		            ".dynstr": 5330,
		            ".rela.dyn": 264,
		            ".rela.plt": 576,
		            ".rodata": 699,
		            ".eh_frame_hdr": 316,
		            ".plt": 400,
		            ".dynamic": 688,
		            ".got": 72,
		            ".got.plt": 216,
		            ".shstrtab": 433,
		            ".strtab": 6322,
		        },
		        "/usr/lib/x86_64-linux-gnu/crt1.o": {
		            ".note.ABI-tag": 32,
		            ".eh_frame": 92,
		            ".text": 50,
		            ".data": 4,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/example/liblib.a": {
		            ".rodata": 197,
		            ".gcc_except_table": 296,
		            ".eh_frame": 584,
		            ".text": 4468,
		            ".data.rel.ro": 104,
		            ".data": 20056,
		            ".bss": 66,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/out/std/libstd.lo": {
		            ".rodata": 85,
		            ".gcc_except_table": 20,
		            ".eh_frame": 140,
		            ".text": 142,
		            ".data.rel.ro": 112,
		            ".data": 8,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/stack/x86_64_system_v/libx86_64_system_v.a":
		            {
		                ".rodata": 1,
		                ".eh_frame": 24,
		                ".text": 138,
		            },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/core/libscheduler.a": {
		            ".gcc_except_table": 72,
		            ".eh_frame": 148,
		            ".text": 934,
		            ".tbss": 104,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/libplatform.a": {
		            ".gcc_except_table": 20,
		            ".eh_frame": 92,
		            ".text": 142,
		        },
		        "/usr/lib/x86_64-linux-gnu/libc_nonshared.a": {
		            ".eh_frame": 96,
		            ".text": 114,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/core/libassert_minimal.a": {
		            ".eh_frame": 128,
		            ".text": 1540,
		            ".bss": 16,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/clock/std/libstd.lo": {
		            ".eh_frame": 72,
		            ".text": 44,
		        },
		        "bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/panic/exit/libexit.lo": {
		            ".eh_frame": 32,
		            ".text": 14,
		        },
		        "/usr/lib/gcc/x86_64-linux-gnu/7.5.0/crtbegin.o": {
		            ".text": 167,
		            ".fini_array": 8,
		            ".init_array": 8,
		            ".bss": 1,
		        },
		        "/usr/lib/x86_64-linux-gnu/crti.o": {
		            ".init": 18,
		            ".fini": 4
		        },
		        "/usr/lib/x86_64-linux-gnu/crtn.o": {
		            ".init": 5,
		            ".fini": 5
		        },
		        "/usr/lib/gcc/x86_64-linux-gnu/7.5.0/crtend.o": {},
		    },
		)

	def testParserNonWorking(self) -> None:
		self.maxDiff = None

		# Non-working cases
		assert self.pathGccX86
		parser1 = ParserClang(self.pathGccX86)
		result = parser1.parse()
		self.assertFalse(result)

		assert self.pathGccEsp32XtensaLx6
		parser2 = ParserClang(self.pathGccEsp32XtensaLx6)
		result = parser2.parse()
		self.assertFalse(result)

		assert self.pathClang
		parser3 = ParserGcc(self.pathClang)
		result = parser3.parse()
		self.assertFalse(result)


if __name__ == "__main__":
	TestRun.pathGccEsp32XtensaLx6 = pathlib.Path(sys.argv.pop())
	TestRun.pathGccX86 = pathlib.Path(sys.argv.pop())
	TestRun.pathClang = pathlib.Path(sys.argv.pop())

	unittest.main()
