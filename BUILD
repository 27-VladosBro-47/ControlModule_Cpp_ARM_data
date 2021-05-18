package(default_visibility = ["//mediapipe/examples:__subpackages__"])

cc_binary(
    name = "controle_module",
    srcs = ["source.cpp"],
    deps = [
        "//mediapipe/examples/desktop/controle_module/hand_tracking:hand_tracking",
        "//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
    ],
)