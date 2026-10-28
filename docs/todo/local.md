# 本项目自身的待办

只记录，不动手（见 `AGENTS.md` 第 1 节）。累积到 5 条以上或出现阻塞项时，会先问是否插一轮代码优化。

归轮：`性能轮` / `功能轮` / `待批准`。

---

### L-001 异常按值捕获

- 类型：性能 + 规范
- 位置：`src/OpenScripter.cpp:46`、`project/OpenScripter.cpp:13`
- 现象：`catch (std::exception e)` 触发对象切片与一次拷贝。
- 期望：`catch (const std::exception &e)`。
- 归轮：性能轮
- 状态：待汇总

### L-002 依赖隐式传递包含

- 类型：可维护性
- 位置：`src/OpenScripter.cpp:48` 使用 `std::cout`，但本文件未 `#include <iostream>`
- 现象：当前能编译，靠的是 `modules/OpenCore/include/Asset/ResourceManager.hpp:16` 的传递包含。
- 期望：自己用到的头文件自己包含。
- 归轮：性能轮（与 OC-002 一起做更省事）
- 状态：待汇总

### L-003 预加载清单不存在，加载失败被当成成功

- 类型：功能缺陷
- 位置：`src/Stage/PreloadStage.cpp:14`（`LoadResourcesFromJson(10001)`）、`modules/OpenCore/include/Core/Macros.hpp:8`（`JSON_PATH "assets/script"`）
- 现象：运行日志报 `JSON file assets/script/STAGE_10001.json does not exist`，但 `LoadingState` 仍返回 `ready`，流程继续推进。
- 期望：先确定清单文件是否该存在；若存在则补文件，若不该存在则删掉这次调用。根因之一在 OC-008。
- 归轮：功能轮
- 状态：待汇总

### L-004 字体 9001 未加载，FrameCounter 画不出字

- 类型：功能缺陷
- 位置：`modules/OpenCore/include/Runtime/Graphics/UI/TextArea.hpp:63`（默认 `fontID = 9001`），`src/Stage/PreloadStage.cpp:53` 创建 `FrameCounter`
- 现象：运行日志两次报 `GetFont failed to get font id 9001`。
- 期望：补字体资源与清单，或显式指定已加载的字体 id。
- 归轮：功能轮（等资源清单方案确定后再动）
- 状态：待汇总

### L-005 预加载结束后没有任何舞台接手

- 类型：功能缺陷
- 位置：`src/Stage/PreloadStage.cpp:62`
- 现象：`PreloadStage` 是唯一的 overlay stage，`removeStage(StageType::overlayStage)` 之后 base/top 槽均为空，程序进入空白窗口空转（2026-10-28 实测：进程 5 秒后仍存活，画面无内容）。
- 期望：预加载完成后切入真正的设计器主舞台。
- 归轮：功能轮（会被主界面实现自然取代）
- 状态：待汇总

### L-006 子模块落后 55 个提交

- 类型：版本
- 位置：`modules/OpenCore`，当前 `970a9f6`，`origin/main` 为 `df9968c`，落后 55、领先 0。
- 影响：缺 `string_view` 系列 API 变更、`TextButton`、`ToolMenu`、`PackageManager`、`RscAudio/RscAsset`，以及 `constraints/OpenCore AI 代码生成优化规范.md`。
- 升级后确定会断的四处（只读 diff 核对，未编译验证）：
  1. `Stage(Timer*, StageManager*, StageType)` 构造器已删除，改为默认构造 + 由 `StageManager::changeStage` 调用 `configure(timer, this)` 注入。影响 `include/OpenScripter/Stage/PreloadStage.hpp:23` 与 `src/OpenScripter.cpp:23`。
  2. `UI<T>(id, layer, textureName, frameX, frameY)` 的第三参数由 `short texID` 变为 `string_view textureName`，经 `PackageManager::getTextureObject` 解析。影响 `src/Stage/PreloadStage.cpp:53` 的 `UI<FrameCounter>("frameCounter", 100, 0, 0, 0)`。
  3. `ResourceManager` 已从 `include/Asset/` 移除，`OpenCoreManagers` 只剩 `ThrManager/EvtManager/GFXManager`。影响 `src/Stage/PreloadStage.cpp:14` 的 `ResManager.LoadResourcesFromJson(10001)`，需整体改为 `PackageManager` 的命名资源与 `registerFolder`。
  4. `Macros.hpp` 的 `JSON_PATH` 需重新确认是否还存在，资源根约定随之改写（牵连 L-008）。
- 连带影响：L-003、L-004、L-008 的方案都要跟着 `PackageManager` 重做，不是补两个文件就能收尾。
- 期望：升级到 `origin/main` 最新提交后重新构建验证。
- 归轮：待批准（子模块变更需你批准，且我不执行 checkout）
- 状态：待在批准后执行

### L-007 格式化规则不完整

- 类型：规范
- 位置：`.clang-format`（仅 `BreakBeforeBraces: Allman`、`IndentWidth: 4`）
- 现象：`ColumnLimit`、指针绑定、`SortIncludes` 等均为 LLVM 默认值，未与本项目实际写法对齐。
- 期望：补全规则后再统一格式化，避免格式化产生大面积无关 diff。
- 归轮：待批准（会触发全仓库重排）
- 状态：待汇总

### L-008 资源目录约定不清

- 类型：规范
- 位置：`CMakeLists.txt:40-45` 把 `assets/` 整目录复制到 exe 旁；实际产物是 `build/assets/assets/ui/icon.png` 与 `build/assets/Raw/`，而清单路径是 `build/assets/script/`。
- 现象：资源根下同时存在 `assets/`（游戏资源）、`Raw/`（源文件）与未来的 `script/` 三种命名，层级含义不明。
- 期望：明确资源根与各子目录职责，写入文档后再建目录。
- 归轮：功能轮（先等脚本格式定下来）
- 状态：待汇总
