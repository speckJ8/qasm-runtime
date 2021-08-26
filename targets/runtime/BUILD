cc_library (
    name = "qgate",
    srcs = [ "qgate.cc" ],
    hdrs = [ "qgate.h" ],
)

#cc_library (
#    name = "qubit",
#    srcs = [ "qubit.cc" ],
#    hdrs = [ "qubit.h", "qgate.h" ],
#)

cc_library (
    name = "state",
    hdrs = [ "state.h" ],
    srcs = [ "state.cc" ],
    deps = [ "qgate" ],
)

cc_library (
    name = "runtime",
    hdrs = [ "runtime.h" ],
    srcs = [ "runtime.cc" ],
    deps = [ "qgate", "state" ],
    visibility = [ "//visibility:public" ],
)
