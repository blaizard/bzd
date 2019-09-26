cc_library(
    name = "include",
    hdrs = glob([
		"**/*.h"
	]),
	visibility = ["//visibility:public"]
)

cc_test(
    name = "tests",
    srcs = glob([
		"**/*.cpp"
	]),
  deps = [
	  ":include",
    "@googletest//:gtest_main",
  ],
)
