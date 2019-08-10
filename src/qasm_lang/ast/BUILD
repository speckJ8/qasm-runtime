cc_library (
    name = "expression",
    hdrs = [ "expression.h" ],
    srcs = [ "expression.cc" ],
)

cc_library (
    name = "variable",
    hdrs = [ "variable.h" ],
    srcs = [ "variable.cc" ],
    deps = [ "expression" ],
)

cc_library (
    name = "statement",
    hdrs = [ "statement.h" ],
    srcs = [ "statement.cc" ],
    deps = [ "expression", "variable" ],
)

cc_library (
    name = "program",
    hdrs = [ "program.h" ],
    srcs = [ "program.cc" ],
    deps = [ "statement"],
)

cc_library (
    name = "node",
    hdrs = [ "node.h" ],
    deps = [ "//third_party:pegtl" ],
    visibility = [ "//src/qasm_lang:__pkg__" ],
)

cc_library (
    name = "ast",
    hdrs = [ "ast.h" ],
    deps = [ "program", "statement", "expression", "variable", "node", ],
    visibility = [ "//src/qasm_lang:__pkg__" ],
)
