cc_library (
    name = "expression",
    hdrs = [ "expression.h" ],
    srcs = [ "expression.cc" ],
    deps = [
        "context",
        "//qasm:sema_error",
        "//qasm:symbol_table",
    ]
)

cc_library (
    name = "statement",
    hdrs = [ "statement.h" ],
    srcs = [ "statement.cc" ],
    deps = [ "context", "expression", "list", "//qasm:symbol_table", ],
)

cc_library (
    name = "program",
    hdrs = [ "program.h" ],
    srcs = [ "program.cc" ],
    deps = [ "statement" ],
)

cc_library (
    name = "list",
    hdrs = [ "list.h" ],
    srcs = [ "list.cc" ],
    deps = [ "expression", "context" ],
)

cc_library (
    name = "context",
    hdrs = [ "context.h" ],
    srcs = [ "context.cc" ],
)

cc_library (
    name = "ast",
    hdrs = [ "ast.h" ],
    deps = [ "program", "statement", "expression", "list", ],
    visibility = [ "//qasm:__pkg__" ],
)
