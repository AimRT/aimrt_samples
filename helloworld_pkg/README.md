# helloword

## 构建和安装 (Build and Install)

```
./build.sh
```

## 运行 (Run)

```
cd build
./start_helloworld.sh
```

## 说明

- 此示例创建了一个 `HelloWorldModule`，会在`Initialize`时读取自定义模块配置并打印出来，同时还会在其 `Initialize`、`Start`、`Shutdown`的阶段各打印一行日志；
- 此示例使用 App 模式，在 main 函数中启动 `AimRTCore` 实例，并将 `HelloWorldModule` 注册到其中；
- 可以在启动后观察控制台打印出来的日志，了解框架运行情况；

## 目录结构

```shell
├── build.sh # 构建脚本
├── cmake
│   └── GetAimRT.cmake # 获取 AimRT
├── CMakeLists.txt
├── README.md
└── src
    ├── app # 应用程序入口
    │   └── helloworld_app
    │       ├── CMakeLists.txt
    │       └── main.cc # 主函数
    ├── CMakeLists.txt
    ├── install
    │   └── bin
    │       ├── cfg # 配置文件
    │       │   └── helloworld_cfg.yaml
    │       └── start_helloworld.sh # 启动脚本
    └── module # 业务模块
        └── helloworld_module
            ├── CMakeLists.txt
            ├── helloworld_module.cc
            └── helloworld_module.h
```
