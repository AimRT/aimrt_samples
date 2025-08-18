# my_pb_channel

## 构建和安装 (Build and Install)

```
./build.sh
```

## 运行 (Run)

```
cd build
./start_my_pb_channel.sh
```

## 说明

- 此示例使用自定义的`protobuf`协议， 定义了[`EventMsg`](./src/protocols/event.proto)类型的消息。
- 此示例通过 `Channel` 通信方式，实现以`example/test_topic`为话题的发布者和订阅者之间的通信
- 此示例创建了两个模块： [`NormalPublisherModule`](./src/module/normal_publisher_module)和 [`NormalSubscriberModule`](./src/module/normal_subscriber_module)。
  - 在 `NormalPublisherModule`的`Initialize`阶段会注册所需协议类型的发布者；
  - 在 `NormalPublisherModule`的`Start`阶段会在定义好的执行器中启动 `MainLoop` 函数， 周期性地发布`EventMsg`类型的消息；
  - 在 `NormalSubscriberModule`的`Initialize`阶段会注册所需协议类型的订阅者以及回调函数；
- 此示例使用 App 模式，在 [main](./src/app/my_pb_channel_app/main.cc) 函数中启动 `AimRTCore` 实例，并将 `NormalPublisherModule`和 `NormalSubscriberModule` 注册到其中；
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
    │   └── my_pb_channel_app
    │       ├── CMakeLists.txt
    │       └── main.cc # 主函数
    ├── CMakeLists.txt
    ├── install
    │   └── bin
    │       ├── cfg # 配置文件
    │       │   └── my_pb_channel_cfg.yaml
    │       └── start_my_pb_channel.sh # 启动脚本
    ├── module # 业务模块
    │   ├── normal_publisher_module # 发布模块
    │   │   ├── CMakeLists.txt
    │   │   ├── normal_publisher_module.cc
    │   │   └── normal_publisher_module.h
    │   └── normal_subscriber_module # 订阅模块
    │       ├── CMakeLists.txt
    │       ├── normal_subscriber_module.cc
    │       └── normal_subscriber_module.h
    └── protocols # 协议
        ├── CMakeLists.txt
        └── event.proto
```
