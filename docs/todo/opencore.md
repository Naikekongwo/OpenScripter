# 给 OpenCore 的需求清单

汇总说明见 `AGENTS.md` 第 3、6 节。本文件只记录需要在 **OpenCore 仓库**里新增或修改的事物；本项目内的临时实现见 `local.md` 对应条目。

条目状态取值：`待汇总` / `已并入 vX.Y` / `已接入并移除项目层实现`。

---

### OC-001 日志格式化语义不一致

- 类型：缺陷
- 位置：`modules/OpenCore/src/StageManager.cpp:78`
- 现象：`LOG("Removing stage at index %zu", index)` 原样输出 `%zu`。`LOG` 内部走 `std::format`，占位符应为 `{}`。
- 证据：2026-10-28 运行 `OpenScripter.exe`，stderr 出现 `Removing stage at index %zu`。
- 影响：日志误导；同一宏在两套格式化语义之间摇摆，后续极易复发。
- 期望：统一为 `std::format` 语义，或在规范中明确禁止 printf 风格占位符。
- 阻塞：否
- 状态：待汇总

### OC-002 头文件用全局 using 污染命名空间

- 类型：改进
- 位置：`include/World/Stage/StageManager.hpp:13`、`include/Core/Info/GameInfo.hpp:11`、`include/Core/Math/OpenCore_Wave.hpp:14`、`include/Runtime/Graphics/IDrawableObject/IDrawableObject.hpp:18`、`include/Asset/TextureLoader.hpp:22`、`include/Asset/ResourceManager.hpp:33`、`include/Runtime/Graphics/UI/TextArea.hpp:21`、`include/Runtime/Graphics/UI/TypeWriter.hpp:24`、`include/Runtime/Animation/Concrete/CollectionAnimation.hpp:7`
- 现象：头文件里出现 `using std::unique_ptr;`，被包含方因此可以在不写 `std::` 的情况下使用 `unique_ptr`。`include/OpenCore.hpp:73`、`:110` 正是依赖这种传递可见性。
- 影响：删除任何一个无关头文件都可能让远处的代码编译失败；包含顺序敏感。
- 期望：头文件内一律写全 `std::`，`using` 只出现在 `.cpp` 或 `using` 声明处在类作用域内。
- 阻塞：否
- 状态：待汇总

### OC-003 UI 层缺文本输入控件

- 类型：新能力
- 现状：`include/Runtime/Graphics/UI/` 下只有 `Button`、`TextButton`、`CheckBox`、`FrameCounter`、`ImageBoard`、`Scrollbar`、`Symbol`、`TextArea`、`ToolMenu`、`TypeWriter`，没有可编辑文本框。
- 需求：单行/多行文本输入，支持光标、选区、`SDL_EVENT_TEXT_INPUT` 组合输入（中文 IME 必需）、退格删除、复制粘贴。
- 用途：脚本设计器的代码编辑区。
- 本地过渡：`src/Compat/` 自绘实现。
- 阻塞：是（编辑器核心交互）
- 状态：待汇总

### OC-004 缺裁剪与滚动视口

- 类型：新能力
- 现状：`ImageBoard`、`TextArea` 直接绘制到渲染器，没有封装 `SDL_RenderSetClipRect` 或离屏目标；`Scrollbar` 只有滚动条控件本身，没有配套的可滚动容器。
- 需求：视口容器，内容超出时裁剪并按偏移绘制，与 `Scrollbar` 联动。
- 用途：长脚本列表、行号栏、属性面板。
- 本地过渡：`src/Compat/`。
- 阻塞：是
- 状态：待汇总

### OC-005 舞台结构固定为 3 槽，缺面板与布局管理

- 类型：新能力
- 位置：`include/World/Stage/StageManager.hpp:85-91`（`stageContainer` 固定 base/overlay/top 三槽，每槽一个 `ElementManager`）
- 需求：多面板/停靠布局与焦点管理，或允许注册新的舞台类型。
- 用途：设计器的主编辑区、资源树、属性面板、日志面板需要并存的布局。
- 本地过渡：在单槽内靠 layer 号堆叠，自建焦点轮转。
- 阻塞：否（可先单面板跑通）
- 状态：待汇总

### OC-006 TextArea 无逐段着色能力

- 类型：新能力
- 位置：`include/Runtime/Graphics/UI/TextArea.hpp`
- 现状：整块文本单一颜色（`m_colorR/G/B`），无等宽排版、无按区间着色。
- 需求：按字符区间着色、制表位、等宽字体排版。
- 用途：语法高亮。
- 本地过渡：`src/Compat/` 自绘。
- 阻塞：否（高亮可后置）
- 状态：待汇总

### OC-007 脚本运行时仍是空壳

- 类型：规范/接口定义
- 位置：`include/Gameplay/AVG/Runtime/ScriptEngine/ScriptEngine.hpp`、`include/Gameplay/AVG/Runtime/ScriptEngine/ScriptLoader.hpp`（`origin/main` 上均只有声明，无成员、无实现）
- 需求：与设计器共同定义脚本数据结构与运行时接口（指令集、条件、跳转、标签），两侧共用一份 schema。
- 用途：设计器的产物必须能被引擎直接执行。
- 阻塞：是（决定设计器产物的目标格式）
- 状态：待汇总

### OC-008 资源清单缺失时不上报失败

- 类型：改进
- 位置：`include/Asset/ResourceManager.hpp:119` `LoadResourcesFromJson`、`src/ResourceManager.cpp`
- 现象：`assets/script/STAGE_10001.json` 不存在时只打印 `Error: JSON file ... does not exist`，返回的 `future` 仍正常完成，调用方无法区分成功与失败。
- 影响：`PreloadStage` 把加载失败当成成功继续推进（见 `local.md` L-003）。
- 期望：以异常或状态值把失败传给调用方。
- 阻塞：否
- 状态：待汇总
