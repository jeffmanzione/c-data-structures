load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "arraylike",
    hdrs = ["arraylike.h"],
)

cc_test(
    name = "arraylike_test",
    size = "small",
    srcs = ["arraylike_test.cc"],
    deps = [
        ":arraylike",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "slist",
    srcs = ["slist.c"],
    hdrs = ["slist.h"],
    deps = [
        ":alist",
        "@memory_wrapper//alloc",
        "@memory_wrapper//debug",
        "@memory_wrapper//util",
    ],
)

cc_library(
    name = "keyed_list",
    srcs = ["keyed_list.c"],
    hdrs = ["keyed_list.h"],
    deps = [
        ":slist",
        "@memory_wrapper//alloc",
        "@memory_wrapper//debug",
        "@memory_wrapper//struct:C_DATA_STRUCTURES_defaults",
        "@memory_wrapper//struct:map",
    ],
)
