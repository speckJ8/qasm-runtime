cc_library (
    name = "config",
    srcs = [ "config.cc" ],
    hdrs = [ "config.h" ],
    deps = [
        "//third_party:cxxopts",
    ]
)

cc_binary (
    name  = "main",
    srcs  = [ "main.cc" ],
    deps  = [
        "config",
        "//qasm:parser",
        "//qasm:sema",
        "//targets/runtime:runtime",
        "//targets/llvm:code_gen",
    ],
    linkopts = [
        # Link options for LLVM
        "-L/usr/lib",
        "-lLLVM-9",
    ]
)
