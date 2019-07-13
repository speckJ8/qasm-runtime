cc_library (
    name = "lexer",
    srcs = [ "lexer.cc" ],
    hdrs = [ "lexer.h" ],
    visibility = [ "//tests:__pkg__" ],
)

cc_library (
    name = "parser",
    srcs = [ "parser.cc" ],
    hdrs = [ "parser.h", "lexer.h" ],
    visibility = [ "//tests:__pkg__" ],
    deps = [
        "//third_party:pegtl"
    ]
)
