cc_library (
    name = "expression",
    hdrs = [ "expression.h" ],
    srcs = [ "expression.cc" ],
)

cc_library (
    name = "statement",
    hdrs = [ "statement.h" ],
    srcs = [ "statement.cc" ],
    deps = [ "expression", "list", "//qasm_lang:symbol_table", ],
)

cc_library (
    name = "program",
    hdrs = [ "program.h" ],
    srcs = [ "program.cc" ],
    deps = [ "statement"],
)

cc_library (
    name = "list",
    hdrs = [ "list.h" ],
    srcs = [ "list.cc" ],
    deps = [ "expression" ],
)

cc_library (
    name = "ast",
    hdrs = [ "ast.h" ],
    deps = [ "program", "statement", "expression", "list", ],
    visibility = [ "//qasm_lang:__pkg__" ],
)
