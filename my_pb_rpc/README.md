# my_pb_rpc

## 构建和安装 (Build and Install)

```
./build.sh
```

## 运行 (Run)

```
cd build
./start_my_rpc.sh
```

## 说明

- 此示例使用自定义的`protobuf`协议， 通过同步 Client 接口向[`CalculationService`](./src/protocols/rpc.proto)发起 Rpc 请求。
- 此示例通过 `Rpc` 通信方式，实现以`example/add_two_num`为函数名客户端和服务端之间的通信
- 此示例创建了两个模块： [`NormalRpcClientModule`](./src/module/normal_client_module)和 [`NormalRpcServerModule`](./src/module/normal_server_module)。
  - 在 `NormalRpcClientModule`的`Initialize`阶段会注册所需协议类型的 client；
  - 在 `NormalRpcClientModule`的`Start`阶段会在定义好的执行器中启动 `MainLoop` 函数， 周期性地发起 Rpc 请求, 其会随机生成两个 0-10 之间的整数， 服务端会返回两数之和；
  - 在 `NormalRpcServerModule`的`Initialize`阶段会注册所需协议类型的 server；
- 此示例使用 App 模式，在 [main](./src/app/my_pb_channel_app/main.cc) 函数中启动 `AimRTCore` 实例，并将 `NormalRpcClientModule`和 `NormalRpcServerModule` 注册到其中；
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
    │   └── my_pb_rpc_app
    │       ├── CMakeLists.txt
    │       └── main.cc # 主函数
    ├── CMakeLists.txt
    ├── install
    │   └── bin
    │       ├── cfg # 配置文件
    │       │   └── my_rpc_cfg.yaml
    │       └── start_my_rpc.sh # 启动脚本
    ├── module # 业务模块
    │   ├── normal_client_module # 客户端模块
    │   │   ├── CMakeLists.txt
    │   │   ├── normal_client_module.cc
    │   │   └── normal_client_module.h
    │   └── normal_server_module # 服务端模块
    │       ├── CMakeLists.txt
    │       ├── global.cc
    │       ├── global.h
    │       ├── normal_server_module.cc
    │       ├── normal_server_module.h
    │       ├── service.cc # 服务端提供的具体服务实现
    │       └── service.h
    └── protocols # 协议
        ├── CMakeLists.txt
        └── rpc.proto
```
