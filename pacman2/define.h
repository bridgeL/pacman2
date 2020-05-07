//zh_CN.GBK
#pragma once
// 定义了二维空间的 point类 和 用于标注区域的 rect类
#include <easyx.h>
#include <conio.h>
#include <time.h>

#include <vector>
#include <iostream>
#include <iterator>
#include <string>
#include <list>
using namespace std;


#define DIR_NONE	 0	// 方向宏定义
#define DIR_UP		-1
#define DIR_DOWN	 1
#define DIR_LEFT	-2
#define DIR_RIGHT	 2


#define BLOCK_SIZE 20			// 方格大小
#define PERSON_SIZE 30			// 人物大小 
#define POINT_SIZE 1			// 豆子大小
#define POINT_BIG_SIZE 3		// 大力丸大小

#define BEAN_NUM 195	// 豆子总数（含大力丸）

#define PACMAN_SPEED 3
#define MONSTER_SPEED_0 1
#define MONSTER_SPEED_1 2
#define MONSTER_FEAR_TIME 600

#define MOVE_TOLERENCE 5		// 转弯容限
#define EAT_TOLERANCE 8			// 吃豆容限

#define MAP_COLUMN 21			// 地图方格列数
#define MAP_ROW 27				// 地图方格行数
#define MAP_CNT (MAP_COLUMN * MAP_ROW)	// 地图方格总数

#define GAME_WIDTH (MAP_COLUMN*BLOCK_SIZE)	// 游戏窗口宽度
#define GAME_HEIGHT (MAP_ROW*BLOCK_SIZE)	// 游戏窗口高度

#define END 1
#define MENU 2
#define GAMING 0


class CPoint
{
public:
	int x;
	int y;
	CPoint() :x(0), y(0) {};
	CPoint(int x, int y) :x(x), y(y) {}
};

class CRect
{
public:
	CPoint site;		// 左上角坐标 = [行，列]
	CPoint shape;		// 区域大小   = [高，宽]
	CRect() :site(CPoint()), shape(CPoint()) {}
	CRect(int x, int y, int h, int w) :site(CPoint(x, y)), shape(CPoint(h, w)) {}
};