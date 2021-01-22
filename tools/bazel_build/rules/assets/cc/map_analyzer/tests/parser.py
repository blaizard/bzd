import sys
import unittest
import typing
import pathlib

from tools.bazel_build.rules.assets.cc.map_analyzer.parser.gcc import ParserGcc
from tools.bazel_build.rules.assets.cc.map_analyzer.parser.clang import ParserClang


class TestRun(unittest.TestCase):

	pathGccEsp32XtensaLx6: typing.Optional[pathlib.Path] = None
	pathGccX86: typing.Optional[pathlib.Path] = None
	pathClang: typing.Optional[pathlib.Path] = None

	def testParserGccEsp32XtensaLx6(self) -> None:

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
			parser.getBySections(), {
			'.iram0.vectors': 1024,
			'.iram0.text': 36087,
			'.dram0.data': 8208,
			'.dram0.bss': 29664,
			'.flash.rodata': 89472,
			'.flash.text': 230350,
			'.xtensa.info': 56
			})
		self.assertDictEqual(
			parser.getByUnits(), {
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libfreertos.a': {
			'.iram0.vectors': 425,
			'.iram0.text': 12004,
			'.dram0.data': 4140,
			'.dram0.bss': 776,
			'.flash.rodata': 1060
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libapp_update.a': {
			'.iram0.text': 109,
			'.dram0.bss': 4,
			'.flash.rodata': 256,
			'.flash.text': 103,
			'.xtensa.info': 56
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libesp32.a': {
			'.iram0.text': 6075,
			'.dram0.data': 1954,
			'.dram0.bss': 106,
			'.flash.rodata': 601,
			'.flash.text': 4336
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libesp_common.a': {
			'.iram0.text': 441,
			'.dram0.data': 16,
			'.dram0.bss': 2240,
			'.flash.rodata': 20,
			'.flash.text': 975
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libheap.a': {
			'.iram0.text': 2364,
			'.dram0.data': 4,
			'.dram0.bss': 4,
			'.flash.text': 647
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libnewlib.a': {
			'.iram0.text': 836,
			'.dram0.data': 152,
			'.dram0.bss': 272,
			'.flash.rodata': 89,
			'.flash.text': 869
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libpthread.a': {
			'.iram0.text': 211,
			'.dram0.data': 16,
			'.dram0.bss': 12,
			'.flash.text': 617
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libsoc.a': {
			'.iram0.text': 6522,
			'.dram0.data': 24,
			'.dram0.bss': 4,
			'.flash.rodata': 3309,
			'.flash.text': 707
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libspi_flash.a': {
			'.iram0.text': 4460,
			'.dram0.data': 520,
			'.dram0.bss': 294,
			'.flash.text': 1019
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libxtensa.a': {
			'.iram0.text': 196
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libbootloader_support.a': {
			'.iram0.text': 1020,
			'.flash.text': 495
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libhal.a': {
			'.iram0.text': 447,
			'.flash.rodata': 32
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libesp_ringbuf.a': {
			'.iram0.text': 858,
			'.flash.rodata': 154
			},
			'/home/blaise/.cache/bazel/_bazel_blaise/487b37b7551c3a6035cab0c6471a2d4b/external/esp32_xtensa_lx6_gcc_8.2.0/lib/gcc/xtensa-esp32-elf/8.2.0/no-rtti/libgcc.a':
			{
			'.iram0.text': 104,
			'.dram0.data': 4,
			'.dram0.bss': 12,
			'.flash.rodata': 872,
			'.flash.text': 5270
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libcxx.a': {
			'.dram0.data': 8,
			'.dram0.bss': 16,
			'.flash.text': 525
			},
			'external/esp32_xtensa_lx6_gcc_8.2.0/xtensa-esp32-elf/lib/libc.a': {
			'.dram0.data': 364,
			'.dram0.bss': 30,
			'.flash.rodata': 4541,
			'.flash.text': 79615
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libvfs.a': {
			'.dram0.data': 308,
			'.dram0.bss': 48,
			'.flash.rodata': 162,
			'.flash.text': 5376
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/example/liblib.a': {
			'.dram0.data': 188,
			'.dram0.bss': 20099,
			'.flash.rodata': 1167,
			'.flash.text': 2765
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/core/libscheduler.a': {
			'.dram0.data': 96,
			'.dram0.bss': 1,
			'.flash.rodata': 571,
			'.flash.text': 608
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/core/libassert_minimal.a': {
			'.dram0.data': 104,
			'.dram0.bss': 13,
			'.flash.rodata': 353,
			'.flash.text': 1055
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/out/std/libstd.lo': {
			'.dram0.data': 40,
			'.dram0.bss': 1,
			'.flash.rodata': 101,
			'.flash.text': 75
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libdriver.a': {
			'.dram0.data': 84,
			'.dram0.bss': 20,
			'.flash.rodata': 280,
			'.flash.text': 2919
			},
			'external/esp32_xtensa_lx6_gcc_8.2.0/xtensa-esp32-elf/lib/libstdc++.a': {
			'.dram0.data': 153,
			'.dram0.bss': 5648,
			'.flash.rodata': 76810,
			'.flash.text': 120088
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/stack/stub/libstub.a': {
			'.flash.rodata': 1,
			'.flash.text': 10
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/libplatform.a': {
			'.flash.rodata': 60,
			'.flash.text': 117
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libmain.a': {
			'.flash.text': 8
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/clock/esp32_xtensa_lx6/libesp32_xtensa_lx6.lo':
			{
			'.flash.text': 26
			},
			'bazel-out/k8-fastbuild_esp32_xtensa_lx6_gcc/bin/cc/bzd/platform/panic/exit/libexit.lo': {
			'.flash.text': 16
			},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libefuse.a': {},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/liblog.a': {},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libmbedtls.a': {},
			'external/esp32_xtensa_lx6_sdk_4.0/lib/libwpa_supplicant.a': {}
			})

	def testParserGccX86(self) -> None:

		assert self.pathGccX86
		parser = ParserGcc(self.pathGccX86)
		result = parser.parse()
		self.assertTrue(result)

		parser.filter(".debug_*")
		parser.filter(".comment")
		parser.filter(".symtab")

		self.assertDictEqual(
			parser.getBySections(), {
			'.interp': 28,
			'.note.ABI-tag': 32,
			'.note.gnu.build-id': 36,
			'.dynsym': 11472,
			'.dynstr': 32813,
			'.gnu.hash': 3376,
			'.gnu.version': 956,
			'.gnu.version_r': 192,
			'.rela.dyn': 1560,
			'.rela.plt': 408,
			'.init': 23,
			'.plt': 288,
			'.text': 24898,
			'.fini': 9,
			'.rodata': 3162,
			'.gcc_except_table': 111,
			'.eh_frame': 12428,
			'.eh_frame_hdr': 3004,
			'.tbss': 104,
			'.data.rel.ro.local': 24,
			'.fini_array': 8,
			'.init_array': 40,
			'.data.rel.ro': 448,
			'.dynamic': 752,
			'.got': 64,
			'.got.plt': 160,
			'.data': 32,
			'.bss': 20273,
			'.note.gnu.gold-version': 28,
			'.strtab': 45531,
			'.shstrtab': 432
			})
		self.assertDictEqual(
			parser.getByUnits(), {
			'/usr/lib/x86_64-linux-gnu/Scrt1.o': {
			'.note.ABI-tag': 32,
			'.text': 43,
			'.data': 4
			},
			'/usr/lib/x86_64-linux-gnu/crti.o': {
			'.init': 18,
			'.fini': 4
			},
			'/usr/lib/x86_64-linux-gnu/crtn.o': {
			'.init': 5,
			'.fini': 5
			},
			'/usr/lib/gcc/x86_64-linux-gnu/7/crtbeginS.o': {
			'.text': 218,
			'.fini_array': 8,
			'.init_array': 8,
			'.data': 8,
			'.bss': 1
			},
			'bazel-out/k8-fastbuild/bin/example/liblib.a': {
			'.text': 16299,
			'.rodata': 1518,
			'.gcc_except_table': 91,
			'.data.rel.ro.local': 8,
			'.init_array': 8,
			'.data.rel.ro': 184,
			'.data': 8,
			'.bss': 20216
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/core/libscheduler.a': {
			'.text': 1334,
			'.rodata': 1043,
			'.tbss': 104,
			'.data.rel.ro.local': 8,
			'.init_array': 8,
			'.bss': 1
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/core/libassert_minimal.a': {
			'.text': 5307,
			'.rodata': 424,
			'.gcc_except_table': 8,
			'.data.rel.ro.local': 8,
			'.init_array': 8,
			'.data.rel.ro': 192,
			'.bss': 17
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/platform/libplatform.a': {
			'.text': 303,
			'.gcc_except_table': 4
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/platform/clock/std/libstd.lo': {
			'.text': 581,
			'.gcc_except_table': 4
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/platform/out/std/libstd.lo': {
			'.text': 264,
			'.rodata': 89,
			'.gcc_except_table': 4,
			'.init_array': 8,
			'.data.rel.ro': 72,
			'.data': 8,
			'.bss': 1
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/platform/panic/exit/libexit.lo': {
			'.text': 14
			},
			'bazel-out/k8-fastbuild/bin/cc/bzd/platform/stack/x86_64_system_v/libx86_64_system_v.a': {
			'.text': 265,
			'.rodata': 1
			},
			'/usr/lib/x86_64-linux-gnu/libc_nonshared.a': {
			'.text': 114
			},
			'/usr/lib/gcc/x86_64-linux-gnu/7/crtendS.o': {
			'.eh_frame': 4
			}
			})

	def testParserClang(self) -> None:

		# Working case
		assert self.pathClang
		parser = ParserClang(self.pathClang)
		result = parser.parse()
		self.assertTrue(result)

		parser.filter(".debug_*")
		parser.filter(".comment")
		parser.filter(".symtab")

		self.assertDictEqual(
			parser.getBySections(), {
			'.interp': 28,
			'.note.ABI-tag': 32,
			'.note.gnu.build-id': 32,
			'.dynsym': 2160,
			'.gnu.version': 180,
			'.gnu.version_r': 112,
			'.gnu.hash': 416,
			'.dynstr': 5330,
			'.rela.dyn': 264,
			'.rela.plt': 576,
			'.rodata': 982,
			'.gcc_except_table': 408,
			'.eh_frame_hdr': 316,
			'.eh_frame': 1492,
			'.text': 8070,
			'.init': 23,
			'.fini': 9,
			'.plt': 400,
			'.tbss': 104,
			'.fini_array': 8,
			'.init_array': 8,
			'.data.rel.ro': 216,
			'.dynamic': 688,
			'.got': 72,
			'.got.plt': 216,
			'.data': 20072,
			'.bss': 104,
			'.shstrtab': 433,
			'.strtab': 6322
			})
		self.assertDictEqual(
			parser.getByUnits(), {
			'<internal>': {
			'.interp': 28,
			'.note.gnu.build-id': 32,
			'.dynsym': 2160,
			'.gnu.version': 180,
			'.gnu.version_r': 112,
			'.gnu.hash': 416,
			'.dynstr': 5330,
			'.rela.dyn': 264,
			'.rela.plt': 576,
			'.rodata': 699,
			'.eh_frame_hdr': 316,
			'.plt': 400,
			'.dynamic': 688,
			'.got': 72,
			'.got.plt': 216,
			'.shstrtab': 433,
			'.strtab': 6322
			},
			'/usr/lib/x86_64-linux-gnu/crt1.o': {
			'.note.ABI-tag': 32,
			'.eh_frame': 92,
			'.text': 50,
			'.data': 4
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/example/liblib.a': {
			'.rodata': 197,
			'.gcc_except_table': 296,
			'.eh_frame': 584,
			'.text': 4468,
			'.data.rel.ro': 104,
			'.data': 20056,
			'.bss': 66
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/out/std/libstd.lo': {
			'.rodata': 85,
			'.gcc_except_table': 20,
			'.eh_frame': 140,
			'.text': 142,
			'.data.rel.ro': 112,
			'.data': 8
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/stack/x86_64_system_v/libx86_64_system_v.a':
			{
			'.rodata': 1,
			'.eh_frame': 24,
			'.text': 138
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/core/libscheduler.a': {
			'.gcc_except_table': 72,
			'.eh_frame': 148,
			'.text': 934,
			'.tbss': 104
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/libplatform.a': {
			'.gcc_except_table': 20,
			'.eh_frame': 92,
			'.text': 142
			},
			'/usr/lib/x86_64-linux-gnu/libc_nonshared.a': {
			'.eh_frame': 96,
			'.text': 114
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/core/libassert_minimal.a': {
			'.eh_frame': 128,
			'.text': 1540,
			'.bss': 16
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/clock/std/libstd.lo': {
			'.eh_frame': 72,
			'.text': 44
			},
			'bazel-out/k8-fastbuild_linux_x86_64_clang/bin/cc/bzd/platform/panic/exit/libexit.lo': {
			'.eh_frame': 32,
			'.text': 14
			},
			'/usr/lib/gcc/x86_64-linux-gnu/7.5.0/crtbegin.o': {
			'.text': 167,
			'.fini_array': 8,
			'.init_array': 8,
			'.bss': 1
			},
			'/usr/lib/x86_64-linux-gnu/crti.o': {
			'.init': 18,
			'.fini': 4
			},
			'/usr/lib/x86_64-linux-gnu/crtn.o': {
			'.init': 5,
			'.fini': 5
			},
			'/usr/lib/gcc/x86_64-linux-gnu/7.5.0/crtend.o': {}
			})

	def testParserNonWorking(self) -> None:

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


if __name__ == '__main__':

	TestRun.pathGccEsp32XtensaLx6 = pathlib.Path(sys.argv.pop())
	TestRun.pathGccX86 = pathlib.Path(sys.argv.pop())
	TestRun.pathClang = pathlib.Path(sys.argv.pop())

	unittest.main()
