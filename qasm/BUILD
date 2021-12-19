cc_library (
    name = "lexer",
    srcs = [ "lexer.cc" ],
    hdrs = [ "lexer.h" ],
    visibility = [
        "//tests:__pkg__",
    ]
)

cc_library (
    name = "parser_error",
    hdrs = [ "parser_error.h" ],
)

cc_library (
    name = "sema_error",
    hdrs = [ "sema_error.h" ],
    visibility = [
        "//qasm/ast:__pkg__",
    ]
)

cc_library (
    name = "parser",
    srcs = [ "parser.cc" ],
    hdrs = [ "parser.h", ],
    visibility = [ "//visibility:public" ],
    deps = [
        "//qasm/ast:ast",
        "lexer",
        "parser_error",
    ]
)

cc_library (
    name = "sema",
    srcs = [ "sema.cc" ],
    hdrs = [ "sema.h", ],
    visibility = [ "//visibility:public" ],
    deps = [
        "//qasm/ast:ast",
    ]
)

cc_library (
    name = "symbol_table",
    srcs = [ "symbol_table.cc" ],
    hdrs = [ "symbol_table.h" ],
    visibility = [ "//qasm/ast:__pkg__" ],

)
