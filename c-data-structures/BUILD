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
    name = "stable_maplike",
    hdrs = ["stable_maplike.h"],
    deps = [
        ":maplike",
        ":stable_arraylike",
    ],
)

cc_test(
    name = "stable_maplike_test",
    size = "small",
    srcs = ["stable_maplike_test.cc"],
    deps = [
        ":stable_maplike",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "stable_arraylike",
    hdrs = ["stable_arraylike.h"],
)

cc_test(
    name = "stable_arraylike_test",
    size = "small",
    srcs = ["stable_arraylike_test.cc"],
    deps = [
        ":stable_arraylike",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "maplike",
    hdrs = ["maplike.h"],
)

cc_test(
    name = "maplike_test",
    size = "small",
    srcs = ["maplike_test.cc"],
    deps = [
        ":maplike",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
