# xushun::json

- xushun
- 2022/11/24

基于C++11的轻量级JSON库。

## 特性

- 基于C++11
- 跨编译器、跨平台
- 符合[标准](https://www.json.org/json-en.html)的JSON解析器、生成器
- 递归下降解析器
- 使用双精度`double`类型存储JOSN_NUMBER类型
- 支持UTF-8、ASCII的JSON文本
- 仅头文件，低使用成本
- 完善的单元测试（使用GoogleTest）


## 安装

```cpp
#include "json.hh"
using json = xushun::json;
```

## 测试

该项目使用GoogleTest进行了完备的单元测试，测试用例在`./test`目录中查看。

GoogleTest版本：v1.12.1，需要支持C++14的编译器。

## 示例

使用示例在`./example`目录中查看。