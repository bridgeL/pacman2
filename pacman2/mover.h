//zh_CN.GBK
#pragma once
// 此文档主要定义了 mover类，以及其派生的 monster类

#include "define.h"
#include "flash.h"
#include "astar.h"

// 引用该库才能使用 TransparentBlt 函数
#pragma comment( lib, "MSIMG32.LIB")

class CMover
{
protected:
	IMAGE* p_background;		// 背景图片的指针
	IMAGE back;					// 为了加快clear的速度，设置back成员储存每次的背景图
	CFlashGroup faces;	// 各方向动画序列的容器

	CRect rect;					// 当前的位置

	CPoint burn;				// 出生地点

	int* map;					// 地图的指针

	double speed;
	double speed_d;

	int new_dir;				// 新的运动方向，加入此属性是为了改善游戏手感
	int dir;					// 运动方向


public:
	CMover() :p_background(NULL), map(NULL), speed(0), speed_d(0), new_dir(DIR_NONE), dir(DIR_NONE) 
	{
		faces = CFlashGroup(EAT_FLASH_TIME);
	}

	// 将初始化分解为一系列函数,请按照顺序执行
	void init_speed(double s)
	{
		// 设置速度
		speed = s;
	}

	void init_map(int* m)
	{
		// 设置地图地址
		map = m;
	}

	void init_rect(CRect r)
	{
		// 设置当前位置
		rect = r;
		burn = rect.site;
	}

	void init_img(IMAGE* p_back, IMAGE* p_face, int r, int c)
	{
		// 设置背景图指针
		p_background = p_back;

		// 根据图片生成四个方向的动画序列
		int h = rect.shape.x;
		int w = rect.shape.y;

		SetWorkingImage(p_face);

		for (int i = 0; i < r; i++)
		{
			CFlash flash;
			for (int j = 0; j < c; j++)
			{
				IMAGE face;
				int x = i * h;
				int y = j * w;
				getimage(&face, y, x, w, h);
				flash.Add(face);
			}
			faces.Add(flash);
		}

		SetWorkingImage(NULL);
	}

	void Reset()
	{
		rect.site = burn;
		dir = DIR_NONE;
		new_dir = DIR_NONE;
	}

	void BugReset()		// Set the dir of monsters that are stuck due to dir=0 to 1 to prevent them from getting stuck.
	{
		dir = 1;
	}

	// 设置/读取 当前位置的左上角坐标
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
		case DIR_DOWN:  faces.SetIdx(3);  break;
		case DIR_UP:	faces.SetIdx(2);  break;
		case DIR_RIGHT: faces.SetIdx(0);  break;
		case DIR_LEFT:	faces.SetIdx(1);  break;
		}
	}

	void Move(double ss)
	{
		int s = (int)ss;
		speed_d += (ss - s);
		if (speed_d >= 1)
		{
			s++;
			speed_d--;
		}

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

		if (new_dir == DIR_NONE)
		{
			dir = DIR_NONE;
			return 1;
		}
		else if (abs(new_dir) != abs(dir) )	// 是否有转向
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


		int s = (int)speed;	// 移动距离

		if (map[i * MAP_COLUMN + j] == 3)	// 墙
		{
			const int t = BLOCK_SIZE - PERSON_SIZE / 2;	// 墙壁的厚度

			// 计算与墙的距离
			int instance = (int)speed;

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
		faces.Update();

		// 转向
		if (Turn())
			SetFlash(dir);

		// 前进
		Go();
	}

private:
	// 透明贴图函数
	// 参数：
	//		dstimg: 目标 IMAGE 对象指针。NULL 表示默认窗体
	//		x, y:	目标贴图位置
	//		srcimg: 源 IMAGE 对象指针。NULL 表示默认窗体
	//		transparentcolor: 透明色。srcimg 的该颜色并不会复制到 dstimg 上，从而实现透明贴图
	void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg, UINT transparentcolor)
	{
		HDC dstDC = GetImageHDC(dstimg);
		HDC srcDC = GetImageHDC(srcimg);
		int w = srcimg->getwidth();
		int h = srcimg->getheight();

		// 使用 Windows GDI 函数实现透明位图
		TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, transparentcolor);
	}

public:
	void Draw()
	{
		// 为了美观，
		// 绘图坐标draw 和 实际坐标site 之间存在固定偏差 bias = BLOCK_SIZE / 2
		// center = site + shape / 2
		// draw = center + bias - shape / 2 
		//      = site + bias
		transparentimage(NULL, rect.site.y + BLOCK_SIZE / 2, rect.site.x + BLOCK_SIZE / 2, &faces.GetImage(), BLACK);
		//putimage(rect.site.y + BLOCK_SIZE / 2, rect.site.x + BLOCK_SIZE / 2, &faces.GetImage());
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
	CPoint escape_rc;	// 逃跑的地点
	CAStar brain;

public:
	CMonster() :CMover(), cd(0), status(0) {}

	void SwitchPathShow(COLORREF color = RED)
	{
		brain.SwitchPathShow(color);
	}

	void SetBrainStyle(int instance, int path)
	{
		brain.SetStyle(instance, path);
	}

	void init_chase_mode(CPoint chase_mode)
	{
		// 设置追击模式
		int instance = chase_mode.x;
		int path = chase_mode.y;
		brain.SetStyle(instance, path);
	}

	void init_img(IMAGE* p_back, IMAGE* p_face, IMAGE* p_dead)
	{
		// 设置背景图指针
		p_background = p_back;

		int h = rect.shape.x;
		int w = rect.shape.y;

		// 根据图片生成四个方向的动画序列
		SetWorkingImage(p_face);
		for (int i = 0; i < 4; i++)
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
			faces.Add(flash);
		}

		// 添加恐惧/死亡的怪物动画
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
			faces.Add(flash);
		}

		for (int i = 2; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				CFlash flash;
				IMAGE face;
				int x = i * h;
				int y = j * w;
				getimage(&face, y, x, w, h);
				flash.Add(face);
				faces.Add(flash);
			}
		}

		SetWorkingImage(NULL);
	}

	void init(IMAGE* p_back, IMAGE* p_face, IMAGE* p_dead, CPoint mn, CRect r, int* mp, double s, int c)
	{
		init_speed(s);

		init_map(mp);

		init_rect(r);

		init_img(p_back, p_face, p_dead);
	}

	void Fear(int t=0)
	{
		if (t > 0)
		{
			if (status != 2)
			{
				cd = t;
				status = 1;
				SetFlash();
			}
		}
		else
		{
			if (status == 1)
			{
				cd--;
				if (cd == 0)
				{
					status = 0;
					escape_rc.x = 0;
					escape_rc.y = 0;
				}


				if (cd >= MONSTER_FEAR_TIME * 0.1)
					faces.SetIdx(4);
				else if (cd % 10 > 5)
					faces.SetIdx(5);
				else
					faces.SetIdx(4);
			}
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
			case DIR_DOWN:  faces.SetIdx(3);  break;
			case DIR_UP:	faces.SetIdx(2);  break;
			case DIR_RIGHT: faces.SetIdx(0);  break;
			case DIR_LEFT:	faces.SetIdx(1);  break;
			}
		}
		else if (status == 2)
		{
			switch (d)
			{
			case DIR_DOWN:  faces.SetIdx(8);  break;
			case DIR_UP:	faces.SetIdx(6);  break;
			case DIR_RIGHT: faces.SetIdx(7);  break;
			case DIR_LEFT:	faces.SetIdx(9);  break;
			}
		}

	}

	int GetStatus() { return status; }
	
	void Update(CMover* p)
	{
		faces.Update();

		Fear();

		// monster的位置
		int mdx, mdy, mi, mj;
		GetNearCross(mdx, mdy, mi, mj);

		// player的位置
		int pdx, pdy, pi, pj;
		p->GetNearCross(pdx, pdy, pi, pj);

		if (status == 0 )
		{
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
			const static int safe_row[7] = { 1,5,8,18,20,23,25 };
			const static int safe_column[6] = { 1,5,9,11,15,19 };

			int danger_row_idx = 0;
			for (int i = 0; i < 6; i++)
				if (safe_row[i] < pi && pi < safe_row[i + 1])
				{
					danger_row_idx = i;
					break;
				}

			int danger_column_idx = 0;
			for (int i = 0; i < 5; i++)
				if (safe_column[i] < pj && pj < safe_column[i + 1])
				{
					danger_column_idx = i;
					break;
				}

			

			if ((escape_rc.x == mi && escape_rc.y == mj ) || (escape_rc.x == 0 && escape_rc.y == 0) 
				||(abs(escape_rc.x - pi) <= 10 && abs(escape_rc.y- pj) <= 5))
			{
				
				while (1)
				{
					int r = rand() % 7;
					if (abs(r - danger_row_idx) > 1)
					{
						escape_rc.x = safe_row[r];
						break;
					}
				}

				while (1)
				{
					int c = rand() % 6;
					if (abs(c - danger_column_idx) > 1)
					{
						escape_rc.y = safe_column[c];
						break;
					}
				}
				//cout << "escape_rc = (" << escape_rc.x << "," << escape_rc.y << ")\n";
			}


			// A* 算法，计算移动方向
			brain.init(map, CPoint(mi, mj), CPoint(escape_rc.x, escape_rc.y));

			if (abs(mi - pi) <= 4 && abs(mj - pj) <= 3)
			{
				for (int i = -1; i < 2; i++)
				{
					for (int j = -1; j < 2; j++)
					{
						brain.SetNodeWall(pi + i, pj + j);
					}
				}
			}
			else
			{
				for (int i = -3; i < 4; i++)
				{
					for (int j = -2; j < 3; j++)
					{
						brain.SetNodeWall(pi + i, pj + j);
					}
				}
			}

			
			brain.BuildAWay();

			int dir = brain.GetDir();
			SetNewDir(dir);

			// 转向
			if (Turn())
				SetFlash(dir);

			// 前进
			Go();
		}
		else
		{
			// A* 算法，计算移动方向
			brain.init(map, CPoint(mi, mj), CPoint(13, 10));
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

	}

	void Reset()
	{
		CMover::Reset();
		cd = 0;
		status = 0;
	}

	void BugReset()
	{
		CMover::BugReset();
	}
};

class CPacman : public CMover
{
private:
	int score;

public:
	CPacman() : CMover(), score(0) {};

	int GetScore() { return score; }

	void AddScore(int ds)
	{
		score += ds;

		TCHAR str[10];
		_itot_s(score, str, 10);

		outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 11 * BLOCK_SIZE, _T("     "));
		outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 11 * BLOCK_SIZE, str);
	}

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

				AddScore(m);

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
			CMonster* mons = mons_list[i];
			CPoint m_s = mons->GetSite();
			int instance = abs(m_s.x - rect.site.x) + abs(m_s.y - rect.site.y);
			if (instance <= FIGHT_TOLERANCE)
			{
				int st = mons->GetStatus();
				if (st == 0)
				{
					return 1;
				}
				else if (st == 1)
				{
					mons->Die();
					AddScore(10);

				}
			}
		}
		return 0;
	}

	void Reset()
	{
		score = 0;
		CMover::Reset();
	}

};