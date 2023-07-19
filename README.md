# REngine

## 总体目标：

1. 渲染引擎
2. 体量小，避免冗余繁杂的设计，体现引擎的功能框架
3. 针对新一代渲染api
4. 设计及实现上应该是清晰简单易懂的
5. 程序应该是健壮的
6. 体现新一代渲染api特性（多线程渲染，多显卡利用，PSO等）
7. 尽量使用标准库或准标准库boost
8. 能够满足未来新技术的快速测试需要
9. 渲染资源来源用UE4导出（houdini usd庞大暂不考虑）


## 设计

### 框架

划分mainthread和renderthread。mainthread负责主要引擎逻辑，renderthread负责渲染相关。

### 命名

### class命名规则

按照UE4引擎的规则，即mainthread的用U打头，renderthread的用F打头

### class成员变量

public用大写字母开头

protect和private用m开头，静态的用ms开头

成员排列按照逻辑层次从高到低排列，相同逻辑层次按照复杂程度从高到低排列，public成员变量排前，其次是protect和private

### class 方法

public用大写字母开头

protect用小写字母开头

private用下划线+小写字母写开头

### shader

函数大写字母打头

全局变量大小字母打头

struct成员变量大小字母打头

函数内局部变量小写字母打头

## 操作控制

WSAD控制相机前后左右移动

上下箭头控制左右移动

左右箭头控制摄像机转向

鼠标按下控制相机旋转

## 目前进展

读取UE4导出的二进制staticmesh

读取场景文件

摄像机控制和UE相同

shadowmap, bloom效果

骨骼动画及融合效果

实现unlit, default lit, subsurface, preintegrated skin, two side foliage, clear coat, cloth(clear coat 和 cloth

截图

![screenshot](Engine/Documentation/Image/defaultlit.png)

![screenshot](Engine/Documentation/Image/skin.png)

![screenshot](Engine/Documentation/Image/subsurface.png)

![screenshot](Engine/Documentation/Image/foliage.png)

![screenshot](Engine/Documentation/Image/clearcoat.png)

![screenshot](Engine/Documentation/Image/cloth.png)

![screenshot](Engine/Documentation/Image/hair.png)