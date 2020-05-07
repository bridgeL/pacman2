# MyPacman

开发软件：VS2019， Win10

使用的图形库 easyx， 版本2020-1-9，下载地址 https://easyx.cn/

使用的图片素材 /pacman2/res  来自于网络



项目github地址: https://github.com/bridgeL/pacman2

作者 bridgeL



**本文已更新对应v1.4.7版本，请在releases中下载对应源码**



#### 游戏核心代码讲解

请移步：https://codebus.cn/bridgel/a/pacman



#### 文档内容

- mover.h
  - CMover
  - CMonster
  - CPacman
- flash.h
  - CFlash
  - CFlashGroup
- define.h
  - 各类宏定义
  - CPoint
  - CRect 
- astar.h
  - 实现a*寻路算法
- game.h/game.cpp
  - 存放各个游戏页的初始化、处理、更新函数
  - 键盘输入捕获线程
  - 定时器线程（控制帧率）
- app.cpp
  - 游戏主循环



#### 目前的问题

还有一些不甚满意的特性存在修改空间：一是四只怪物的追逐模式差异性不大，采用的是调整A*算法中 路径权重和距离权重的比例 的方式实现差异化，希望以后有更好的想法；二是界面仍不够美观，可以继续优化（艺术的事谁说的清呢嘿嘿）；三是可以为这游戏设计更多有趣的特性，例如按X冲刺等；四是最遗憾的，更新了大量代码后，出现了一些难以复现的非恶性bug，尚不足以使游戏崩溃，但时不时出现一次，这令源码蒙上了一层暗尘



#### 彩蛋

代码里初始化怪物时，可以取消一行注释，从而显示怪物的追踪路径（用于Debug查看A*的效果，效果很狂野）