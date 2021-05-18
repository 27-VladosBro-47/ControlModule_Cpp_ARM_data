package(default_visibility = [
    "//visibility:public",
])

cc_library(
    name = "perceptrons",
    srcs = ["perceptrons.cpp"],
    hdrs = ["perceptrons.h"],
)

cc_library(
    name = "neural_network",
    srcs = ["neural_network.cpp"],
    hdrs = ["neural_network.h"],
    deps = [
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:opencv_imgcodecs",
        "//mediapipe/examples/desktop/controle_module/json_lib:json_include",
        ":perceptrons",
    ],

)