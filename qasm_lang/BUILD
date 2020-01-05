cc_library (
    name = "lexer",
    srcs = [ "lexer.cc" ],
    hdrs = [ "lexer.h" ],
)

cc_library (
    name = "parser_error",
    # srcs = [ "parser_error.cc" ],
    hdrs = [ "parser_error.h" ],
)

cc_library (
    name = "parser",
    srcs = [ "parser.cc" ],
    hdrs = [ "parser.h", ],
    visibility = [ "//visibility:public" ],
    deps = [
        "//qasm_lang/ast:ast",
        "lexer",
        "parser_error",
    ]
)

cc_library (
    name = "symbol_table",
    srcs = [ "symbol_table.cc" ],
    hdrs = [ "symbol_table.h" ],
    visibility = [ "//qasm_lang/ast:__pkg__" ],
)
