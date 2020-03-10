#include "bzd/container/btree.h"
#include "cc_test/test.h"

TEST(ContainerBTree, base)
{
	bzd::BTree<int, int, 10, 2> tree;

	tree.insert(12, 22);

	EXPECT_EQ(true, true);
}

/*
 U _ZN3bzd5panicEv

laise@blaise-s730 ~/projects/cpp-coroutines/temp (master) $ nm libplatform_Slib | grep panic
libplatform_Slibbzd_Ucore.so                                             libplatform_Slibraries_Spanic_Sexit_Slibexit.so
libplatform_Slibraries_Sbootstrap_Slibbootstrap.so                       libplatform_Slibraries_Spanic_Sthrow_Slibthrow.so
libplatform_Slibraries_Slog_Sstdout_Slibstdout.so libplatform_Slibraries_Splatforms_Sx86_U64_Ucdecl_Slibx86_U64_Ucdecl.so blaise@blaise-s730
~/projects/cpp-coroutines/temp (master) $ nm libplatform_Slibraries_Spanic_Sexit_Slibexit.so | grep panic 000000000000072a T _ZN3bzd5panicEv
blaise@blaise-s730 ~/projects/cpp-coroutines/temp (master) $ nm libplatform_Slibraries_Spanic_Sthrow_Slibthrow.so | grep panic
00000000000007da T _ZN3bzd5panicEv
blaise@blaise-s730 ~/projects/cpp-coroutines/temp (master) $ ldd libplatform_Slibbzd_Ucore.so
		linux-vdso.so.1 (0x00007ffcae9cc000)
		libc++abi.so.1 =>
/home/blaise/.cache/bazel/_bazel_blaise/487b37b7551c3a6035cab0c6471a2d4b/external/linux_x86_64_clang_9_0_0/lib/libc++abi.so.1
(0x00007f5521b50000) libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f552175f000) /lib64/ld-linux-x86-64.so.2 (0x00007f5521d8a000)
		libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f5521547000)
		libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f5521328000)


blaise@blaise-s730 ~/projects/cpp-coroutines/temp (master) $ ldd btree_test
		libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f962031f000)
		libplatform_Slibbzd_Ucore.so => ./libplatform_Slibbzd_Ucore.so (0x00007f962102a000)
		libtools_Scc_Utest_Slibcc_Utest.so => ./libtools_Scc_Utest_Slibcc_Utest.so (0x00007f9621022000)
		libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f9620100000)
		libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f961fd62000)
		librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f961fb5a000)

ldd btree_test
		libplatform_Slibraries_Slog_Sstdout_Slibstdout.so => ./libplatform_Slibraries_Slog_Sstdout_Slibstdout.so (0x00007f71dfeb5000)
		libplatform_Slibraries_Spanic_Sexit_Slibexit.so => ./libplatform_Slibraries_Spanic_Sexit_Slibexit.so (0x00007f71dfeb2000)
		libplatform_Slibraries_Splatforms_Sx86_U64_Ucdecl_Slibx86_U64_Ucdecl.so =>
./libplatform_Slibraries_Splatforms_Sx86_U64_Ucdecl_Slibx86_U64_Ucdecl.so (0x00007f71dfeaf000) libplatform_Slibbzd_Ucore.so =>
./libplatform_Slibbzd_Ucore.so (0x00007f71dfea8000) libtools_Scc_Utest_Slibcc_Utest.so => ./libtools_Scc_Utest_Slibcc_Utest.so
(0x00007f71dfea2000) libplatform_Slibraries_Sbootstrap_Slibbootstrap.so => ./libplatform_Slibraries_Sbootstrap_Slibbootstrap.so
(0x00007f71dfe9e000) libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f71df375000) libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
(0x00007f71def84000)
*/