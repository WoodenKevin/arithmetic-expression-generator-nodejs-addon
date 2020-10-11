{
    # 构建目标集合
    "targets": [{
        # 模块最终生成的二进制文件名
        "target_name":
        "arithmetic-expression-generator",
        # 要编译的源文件
        "sources": [
            "./src/addon.cc",
        ],
        # 头文件包含目录，!是执行shell命令取输出值，@是在列表中展开输出的每一项
        "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
        # 外部依赖项
        "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
        # 以下是编译器选项，启用node-addon-api的集成C++和JavaScript的异常处理
        "cflags!": ["-fno-exceptions"],
        "cflags_cc!": ["-fno-exceptions"],
        "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "CLANG_CXX_LIBRARY": "libc++",
            "MACOSX_DEPLOYMENT_TARGET": "10.7"
        },
        "msvs_settings": {
            "VCCLCompilerTool": {
                "ExceptionHandling": 1
            }
        },
        # 预定义宏，禁用NAPI的C++异常处理和node-addon-api废弃的API
        "defines":
        ["NAPI_DISABLE_CPP_EXCEPTIONS", "NODE_ADDON_API_DISABLE_DEPRECATED"]
    }]
}
