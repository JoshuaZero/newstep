# NewStep 测控软件框架

基于 Qt + C++ 的测控软件框架，提供美化后的桌面界面、测试执行、整机自检读取、阈值比对与 CSV 报告导出。

## 功能范围

- 上电电压测试
- RAM 测试
- FLASH 测试
- 1553B 通信测试
- RS422 通信测试
- 以太网通信测试
- 开关量输入测试
- 开关量输出测试
- 时序高压测试
- 时序电流测试
- 模拟低压通路
- 配电模拟负载
- 安控指令模拟测试
- 一模故障注入测试
- 两模故障注入测试

每个测试项都会读取整机自检数据并与当前测量值进行一致性比对；一模/两模故障注入测试通过后会显示工作状态正常。

## 构建与安装

依赖：CMake 3.16+、C++17 编译器、Qt 5 Widgets 或 Qt 6 Widgets。

```bash
./scripts/install.sh
```

安装完成后运行：

```bash
./dist/newstep-mct/bin/newstep-mct
```

也可以手动构建：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
```
