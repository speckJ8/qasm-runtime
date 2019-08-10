cc_library (
    name = "lexer",
    srcs = [ "lexer.cc" ],
    hdrs = [ "lexer.h" ],
    visibility = [ "//tests:__pkg__" ],
)

cc_library (
    name = "parser",
    srcs = [ "parser.cc" ],
    hdrs = [ "parser.h", "grammar.h", ],
    visibility = [ "//src:__pkg__" ],
    deps = [
        "//src/qasm_lang/ast:ast"
    ]
)

cc_library (
    name = "symbol_table",
    srcs = [ "symbol_table.cc" ],
    hdrs = [ "symbol_table.h" ],
)
