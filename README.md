# CS1.6中文输入解决方案 - 基于ImGui与OpenGL32



## 🔍 项目简介
为Counter-Strike 1.6添加原生中文输入支持，基于ImGui图形库与OpenGL32实现，解决原版游戏无法输入中文以及崩溃问题。  
⚠️ &zwnj;**仅支持Steam Legacy版本**&zwnj;（非Steam版或其他平台版本可能无法兼容）。

## 🚀 功能特性
✅ 支持全屏/窗口模式下的中文输入  
✅ 集成ImGui实现输入框UI  
✅ 基于OpenGL32的图形渲染  
✅解决了无法输入中文以及开启中文输入法游戏崩溃问题
❌ &zwnj;**不支持**&zwnj;非Steam Legacy版本的CS1.6（如Valve官方重制版或第三方平台版本）  
⚠️ 自担风险使用

## ⚙️ 安装指南
### 测试环境
- Windows 10 22H2 64位系统


## 📜 使用说明
- 编译项目x86，注入dll至游戏
- 启动Steam Legacy版本的CS1.6后，按`回车`激活输入框
- 输入完成后按`Enter`发送
- 按`ESC`关闭输入框

## 📌 重要说明
- &zwnj;**仅兼容Steam Legacy版本**&zwnj;：本工具针对Steam平台的传统CS1.6版本开发，其他版本（如Valve官方重制版或第三方平台版本）可能无法正常工作。
- &zwnj;**风险提示**&zwnj;：
- 项目可能存在BUG
- 由于使用了Hook，会有部分反作弊封禁等风险，开发者不承担任何责任。请自担风险使用。
- 

## 🛠️ 开发环境
- Visual Studio 2026
- win32 x86
- C++17 及以上
- ImGui 1.9.0 docking


## 📜 许可证
本项目采用GPLv3许可证，&zwnj;**禁止商业使用**&zwnj;且&zwnj;**衍生作品必须开源**&zwnj;。



## 📫 联系方式
如有好的建议意见，或是BUG
- 邮箱：1765625096@QQ.COM


