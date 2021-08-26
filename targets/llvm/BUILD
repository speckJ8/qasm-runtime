cc_library (
    name  = "code_gen",
    hdrs  = [ "code_gen.h" ],
    srcs  = [ "code_gen.cc" ],
    deps   = [
        "//qasm/ast:ast",
        "expression"
    ],
    visibility = [ "//visibility:public" ],
)

cc_library (
    name  = "expression",
    hdrs  = [ "expression.h" ],
    srcs  = [ "expression.cc" ],
    deps   = [
        "//qasm/ast:ast",
    ],
)
