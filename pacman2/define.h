//zh_CN.GBK
#pragma once
// �����˶�ά�ռ�� point�� �� ���ڱ�ע����� rect��
#include <easyx.h>
#include <conio.h>
#include <time.h>

#include <vector>
#include <iostream>
#include <iterator>
#include <string>
#include <list>
#include <random>
using namespace std;


#define DIR_NONE	 0	// ����궨��
#define DIR_UP		-1
#define DIR_DOWN	 1
#define DIR_LEFT	-2
#define DIR_RIGHT	 2


#define BLOCK_SIZE 20			// �����С
#define PERSON_SIZE 30			// �����С 
#define POINT_SIZE 1			// ���Ӵ�С
#define POINT_BIG_SIZE 3		// �������С

#define BEAN_NUM 195	// �����������������裩

#define FPS 60
#define PACMAN_SPEED 3
#define MONSTER_SPEED_0 1.5
#define MONSTER_SPEED_1 2.5
#define MONSTER_FEAR_TIME 600
#define EAT_FLASH_TIME 5

#define MOVE_TOLERENCE 5		// ת������
#define EAT_TOLERANCE 8			// �Զ�����
#define FIGHT_TOLERANCE 12		// ս������

#define MAP_COLUMN 21			// ��ͼ��������
#define MAP_ROW 27				// ��ͼ��������
#define MAP_CNT (MAP_COLUMN * MAP_ROW)	// ��ͼ��������

#define GAME_WIDTH (MAP_COLUMN*BLOCK_SIZE)	// ��Ϸ���ڿ��
#define GAME_HEIGHT (MAP_ROW*BLOCK_SIZE)	// ��Ϸ���ڸ߶�

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
	CPoint site;		// ���Ͻ����� = [�У���]
	CPoint shape;		// �����С   = [�ߣ���]
	CRect() :site(CPoint()), shape(CPoint()) {}
	CRect(int x, int y, int h, int w) :site(CPoint(x, y)), shape(CPoint(h, w)) {}
};