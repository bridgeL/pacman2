//zh_CN.GBK
#pragma once
// 此文档主要定义了 mover类，以及其派生的 monster类
// 此外还定义了二维空间的 point类 和 用于标注区域的 rect类

#include "define.h"
#include "flash.h"
#include "astar.h"

class CMover
{
protected:
	IMAGE* p_background;		// 背景图片的指针
	IMAGE back;					// 为了加快clear的速度，设置back成员储存每次的背景图
	CFlashGroup flash_group;	// 各方向动画序列的容器

	CRect rect;					// 当前的位置

	int* map;					// 地图的指针

	int speed;

	int new_dir;				// 新的运动方向，加入此属性是为了改善游戏手感
	int dir;					// 运动方向


public:
	CMover() :p_background(NULL), map(NULL), speed(0), new_dir(DIR_NONE), dir(DIR_NONE) {}

	// 通过一张图片初始化mover对象
	void init(IMAGE* p_back, IMAGE* p_face, CPoint mn, CRect r, int* mp, int s)
	{
		// 设置速度
		speed = s;

		// 设置地图地址
		map = mp;

		// 设置当前位置
		rect = r;

		// 设置背景图指针
		p_background = p_back;

		// 根据图片生成四个方向的动画序列
		int h = rect.shape.x;
		int w = rect.shape.y;

		int m = mn.x;	// 行数  4  4
		int n = mn.y;	// 列数  3  2

		SetWorkingImage(p_face);

		for (int i = 0; i < m; i++)
		{	
			CFlash flash;
			for (int j = 0; j < n; j++)
			{
				IMAGE face;
				int x = i * h;
				int y = j * w;
				getimage(&face, y, x, w, h);
				flash.Add(face);
			}
			flash_group.Add(flash);
		}

		SetWorkingImage(NULL);
	}

	// 设置/读取 当前位置的左上角坐标
	void SetSite(CPoint s) { rect.site = s; }
	CPoint GetSite() { return rect.site; }

	// 读取 当前位置的中心点坐标
	CPoint GetCenter()
	{
		CPoint center;
		center.x = rect.site.x + rect.shape.x / 2;
		center.y = rect.site.y + rect.shape.y / 2;
		return center;
	};

	void SetFlash(int d)
	{
		switch (d)
		{
		case DIR_DOWN:  flash_group.SetIdx(3);  break;
		case DIR_UP:	flash_group.SetIdx(2);  break;
		case DIR_RIGHT: flash_group.SetIdx(0);  break;
		case DIR_LEFT:	flash_group.SetIdx(1);  break;
		}
	}

	void Move(int s)
	{
		switch (dir)
		{
		case DIR_DOWN:  rect.site.x += s; break;	// 下
		case DIR_RIGHT: rect.site.y += s; break;	// 右
		case DIR_UP:	rect.site.x -= s; break;	// 上
		case DIR_LEFT:	rect.site.y -= s; break;	// 左
		}
	}

	void SetNewDir(int d)
	{
		new_dir = d;
	}

	void GetNearCross(int& dx, int& dy, int& i, int& j)
	{
		CPoint c = GetCenter();

		// 距离最靠近的格点的距离
		dx = (c.x + BLOCK_SIZE / 2) % BLOCK_SIZE - BLOCK_SIZE / 2;
		dy = (c.y + BLOCK_SIZE / 2) % BLOCK_SIZE - BLOCK_SIZE / 2;

		int x = c.x - dx;
		int y = c.y - dy;

		i = x / BLOCK_SIZE;
		j = y / BLOCK_SIZE;
	}

	bool Turn()
	{
		// 转向判定
		// 我们要求吃豆人像火车一样，总是位于固定的轨道上，不能自由移动到随意的坐标，
		// 因此在转向的时候，需要进行严格的审查


		// 是否有转向，
			// 是否在转弯容限内，
				// 转弯后是否撞墙
					// 均通过则正确转弯，按新方向前进，并更新动画序列
				// 撞墙了，不许转弯，按旧方向前进
			// 不在转弯容县内，不许转弯，按旧方向前进
		// 没有转向，按旧方向前进

		if (abs(new_dir) * abs(dir) == 2)	// 是否有转向
		{
			// 转弯限制
			const int t = MOVE_TOLERENCE;	// 宽容门限

			int dx, dy, i, j;
			GetNearCross(dx, dy, i, j);

			bool turn_flag = 0;

			if ((abs(new_dir) == 2 && abs(dx) < t) || (abs(new_dir) == 1 && abs(dy) < t))
				turn_flag = 1;

			if (turn_flag)		// 是否在转弯容限内
			{
				// 下一个格点
				switch (new_dir)
				{
				case DIR_DOWN:  i++;  break;
				case DIR_UP:	i--;  break;
				case DIR_RIGHT: j++;  break;
				case DIR_LEFT:	j--;  break;
				}

				bool edge_flag = 0;
				if (j < 0 || j >= MAP_COLUMN)
					edge_flag = 1;

				if (!edge_flag)	// 是否在边界
				{
					bool wall_flag = 0;

					if (map[i * MAP_COLUMN + j] == 3)
						wall_flag = 1;

					if (!wall_flag)	// 转弯后是否撞墙
					{
						// 方向赋值，并修改动画

						dir = new_dir;	

						// 坐标修正
						rect.site.x -= dx;
						rect.site.y -= dy;

						return 1;
					}
					else
					{
						return 0;
					}

				}
				else	// 如果超过边界，不许转弯
				{
					return 0;
				}

			}
			else	// 转弯后撞墙，不许转弯
			{
				return 0;
			}

		}
		else	// 可能是掉头或者从DIR_NONE起步
		{
			dir = new_dir;
			return 1;
		}
	}

	bool Go()
	{
		// 计算离自己最近的节点，并知道自己与此节点的相对位置
		// 根据 移动方向 和 与节点的相对位置, 下一个节点的坐标
		// 计算自己与下一个节点的距离
		// 判断三种状态
			// 不会撞到/不是墙
			// 会撞到，但仍然有一定距离
			// 已经撞到了，没有距离

		int dx, dy, i, j;
		GetNearCross(dx, dy, i, j);

		switch (dir)
		{
		case DIR_DOWN:  if (dx >= 0) i++; break;	// 下
		case DIR_RIGHT: if (dy >= 0) j++; break;	// 右
		case DIR_UP:	if (dx <= 0) i--; break;	// 上
		case DIR_LEFT:	if (dy <= 0) j--; break;	// 左
		}

		// 如果超过边界，就不进行撞墙检查
		if (j < 0 || j >= MAP_COLUMN)
		{
			// 移动
			Move(speed);

			if (rect.site.y < 0)
				rect.site.y += GAME_WIDTH;
			else if (rect.site.y >= GAME_WIDTH)
				rect.site.y -= GAME_WIDTH;

			return 1;
		}


		int s = speed;	// 移动距离

		if (map[i * MAP_COLUMN + j] == 3)	// 墙
		{
			const int t = BLOCK_SIZE - PERSON_SIZE / 2;	// 墙壁的厚度

			// 计算与墙的距离
			int instance = speed;

			switch (dir)
			{
			case DIR_DOWN:  instance = (i * BLOCK_SIZE - t) - (rect.site.x + rect.shape.x);	break;
			case DIR_RIGHT: instance = (j * BLOCK_SIZE - t) - (rect.site.y + rect.shape.y); break;
			case DIR_UP:	instance = (rect.site.x) - (i * BLOCK_SIZE + t);				break;
			case DIR_LEFT:	instance = (rect.site.y) - (j * BLOCK_SIZE + t);				break;
			}

			
			if (instance == 0)
			{
				return 0;
			}

			s = instance;
		}

		// 移动
		Move(s);

		return 1;
	}

	void Update()
	{
		flash_group.Update();

		// 转向
		if (Turn())
			SetFlash(dir);

		// 前进
		Go();
	}

	void Draw()
	{
		// 为了美观，
		// 绘图坐标draw 和 实际坐标site 之间存在固定偏差 bias = BLOCK_SIZE / 2
		// center = site + shape / 2
		// draw = center + bias - shape / 2 
		//      = site + bias
		putimage(rect.site.y + BLOCK_SIZE / 2, rect.site.x + BLOCK_SIZE / 2, &flash_group.GetImage());
	}

	void Clear()
	{
		SetWorkingImage(p_background);
		getimage(&back, rect.site.y + BLOCK_SIZE / 2, rect.site.x + BLOCK_SIZE / 2, rect.shape.y, rect.shape.x);

		SetWorkingImage(NULL);
		putimage(rect.site.y + BLOCK_SIZE / 2, rect.site.x + BLOCK_SIZE / 2, &back);
	}

	int GetDir() { return dir; }

};

class CMonster :public CMover
{
private:
	int cd;
	int status;			// 0追逐、1恐惧、2死亡
	int mode;			// 不同mode追逐模式不同
	CAStar brain;

public:
	CMonster() :CMover(), cd(0), status(0), mode(0) {}

	void SwitchPathShow(COLORREF color = RED)
	{
		brain.SwitchPathShow(color);
	}

	void SetBrainStyle(int instance, int path)
	{
		brain.SetStyle(instance, path);
	}

	void init(IMAGE* p_back, IMAGE* p_face, IMAGE* p_dead, CPoint mn, CRect r, int* mp, int s, int chase_mode)
	{
		CMover::init(p_back, p_face, mn, r, mp, s);

		mode = chase_mode;

		// 添加恐惧/死亡的怪物动画
		int h = rect.shape.x;
		int w = rect.shape.y;

		SetWorkingImage(p_dead);
		for (int i = 0; i < 2; i++)
		{
			CFlash flash;
			for (int j = 0; j < 2; j++)
			{
				IMAGE face;
				int x = i * h;
				int y = j * w;
				getimage(&face, y, x, w, h);
				flash.Add(face);
			}
			flash_group.Add(flash);
		}
		SetWorkingImage(NULL);
	}

	void Fear(int t)
	{
		if (status != 2)
		{
			cd = t;
			status = 1;
			SetFlash();
		}
	}

	void Fear()
	{
		if (status == 1)
		{
			cd--;
			if (cd == 0)
				status = 0;
		}
	}

	void Die()
	{
		status = 2;
		SetFlash();
	}

	void Burn()
	{
		status = 0;
		SetFlash(dir);
	}

	void SetFlash(int d=0)
	{
		if (status == 0)
		{
			switch (d)
			{
			case DIR_DOWN:  flash_group.SetIdx(3);  break;
			case DIR_UP:	flash_group.SetIdx(2);  break;
			case DIR_RIGHT: flash_group.SetIdx(0);  break;
			case DIR_LEFT:	flash_group.SetIdx(1);  break;
			}
		}
		else if (status == 1)
		{
			flash_group.SetIdx(4);
		}
		else
		{
			flash_group.SetIdx(5);
		}

	}

	int GetStatus() { return status; }
	
	void Update(CMover* p)
	{
		Fear();

		int mdx, mdy, mi, mj;
		GetNearCross(mdx, mdy, mi, mj);

		if (status == 0 )
		{
			// player的位置
			int pdx, pdy, pi, pj;
			p->GetNearCross(pdx, pdy, pi, pj);

			/**
			if (mode == 1)
			{
				// 各种策略
				int pdir = p->GetDir();
				switch (pdir)
				{
				case DIR_DOWN:  pi+=4;  break;
				case DIR_UP:	pi-=4;  break;
				case DIR_RIGHT: pj+=4;  break;
				case DIR_LEFT:	pj-=4;  break;
				}

				if (map[pi * MAP_COLUMN + pj] == 3)
				{
					for (int i = -1; i < 2; i++)
					{
						for (int j = -1; j < 2; j++)
						{
							if (map[(pi - i) * MAP_COLUMN + pj - j] != 3)
							{
								pi -= i;
								pj -= j;
								i = 3;
								break;
							}
						}
					}
				}
			}
			else if (mode == 2)
			{
				// 各种策略
				int pdir = p->GetDir();
				switch (-pdir)
				{
				case DIR_DOWN:  pi += 2;  break;
				case DIR_UP:	pi -= 2;  break;
				case DIR_RIGHT: pj += 2;  break;
				case DIR_LEFT:	pj -= 2;  break;
				}

				if (map[pi * MAP_COLUMN + pj] == 3)
				{
					for (int i = -1; i < 2; i++)
					{
						for (int j = -1; j < 2; j++)
						{
							if (map[(pi - i) * MAP_COLUMN + pj - j] != 3)
							{
								pi -= i;
								pj -= j;
								i = 3;
								break;
							}
						}
					}
				}
			}
			else if (mode == 3)
			{
				if (abs(pi - mi) < 2 && abs(pj - mj) < 2)
				{
					pi = 30;
					pj = 1;
				}
			}
			/**/

			// A* 算法，计算移动方向
			brain.init(map, CPoint(mi, mj), CPoint(pi, pj));
			brain.BuildAWay();

			int dir = brain.GetDir();
			SetNewDir(dir);
				
			// 转向
			if (Turn())
				SetFlash(dir);

			// 前进
			Go();
		}
		else if (status == 1)
		{
			// player的位置
			int pdx, pdy, pi, pj;
			p->GetNearCross(pdx, pdy, pi, pj);


			// A* 算法，计算移动方向
			brain.init(map, CPoint(mi, mj), CPoint(pi, pj));
			brain.BuildAWay();

			int dir = brain.GetDir();

			const int dirs[4] = { DIR_UP,DIR_RIGHT,DIR_DOWN,DIR_LEFT };

			for (int i = 0; i < 4; i++)
			{
				if (dir == dirs[i])
					continue;
				SetNewDir(dirs[i]);

				// 转向
				if (Turn())
					SetFlash(dir);

				// 前进
				if (Go())
					break;
			}
		}
		else
		{
			// A* 算法，计算移动方向
			brain.init(map, CPoint(mi, mj), CPoint(14, 14));
			brain.BuildAWay();

			int dir = brain.GetDir();

			if (dir == DIR_NONE)
				Burn();
			else
				SetNewDir(dir);

			// 转向
			if (Turn())
				SetFlash(dir);

			// 前进
			Go();
		}

		flash_group.Update();

		

	}

	void Reset()
	{
		cd = 0;
		status = 0;
		dir = DIR_NONE;
		new_dir = DIR_NONE;
	}
};

class CPacman : public CMover
{
private:
	int score;

public:
	CPacman() : CMover(), score(0) {};

	int GetScore() { return score; }

	int Eat()	// 吃豆
	{
		const int t = EAT_TOLERANCE;

		// 节点位置
		int dx, dy, i, j;
		GetNearCross(dx, dy, i, j);

		if (abs(dx) + abs(dy) < t)
		{
			int m = map[i * MAP_COLUMN + j];
			int r = 0;
			if (m == 1)
			{
				r = POINT_SIZE;
			}
			else if (m == 2)
			{
				r = POINT_BIG_SIZE;
			}

			if (r != 0)
			{
				map[i * MAP_COLUMN + j] = 0;
				score += m;

				TCHAR str[10];
				_itot_s(score, str, 10);

				outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 12 * BLOCK_SIZE + BLOCK_SIZE / 2, _T("     "));
				outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 12 * BLOCK_SIZE + BLOCK_SIZE / 2, str);

				SetWorkingImage(p_background);
				setfillcolor(BLACK);
				solidcircle(j * BLOCK_SIZE + BLOCK_SIZE / 2, i * BLOCK_SIZE + BLOCK_SIZE / 2, r);
				SetWorkingImage(NULL);

				return m;
			}
		}

		return 0;
	}

	bool Fight(CMonster** mons_list, int num)		// 吃怪物
	{
		const int t = 49;

		int pdx, pdy, pi, pj;
		GetNearCross(pdx, pdy, pi, pj);

		for (int i = 0; i < num; i++)
		{
			int mdx, mdy, mi, mj;
			CMonster* mons = mons_list[i];

			mons->GetNearCross(mdx, mdy, mi, mj);

			if (mi == pi && mj == pj)
			{
				int insx = mdx * pdx;
				int insy = mdy * pdy;
				if (insx >= 0 && insx <= t && insy >= 0 && insy <= t)
				{
					int st = mons->GetStatus();
					if (st == 0)
					{
						return 1;
					}
					else if (st == 1)
					{
						score += 10;
						mons->Die();

						TCHAR str[10];
						_itot_s(score, str, 10);

						outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 12 * BLOCK_SIZE + BLOCK_SIZE / 2, _T("     "));
						outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 12 * BLOCK_SIZE + BLOCK_SIZE / 2, str);
					}
				}
			}
		}
		return 0;
	}

	void Reset()
	{
		score = 0;
		dir = DIR_NONE;
		new_dir = DIR_NONE;
	}
};