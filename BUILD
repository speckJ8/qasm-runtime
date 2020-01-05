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
        "//qasm_lang:parser"
    ],
)
