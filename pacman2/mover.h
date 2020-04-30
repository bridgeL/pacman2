//zh_CN.GBK
#pragma once
// ���ĵ���Ҫ������ mover�࣬�Լ��������� monster��
// ���⻹�����˶�ά�ռ�� point�� �� ���ڱ�ע����� rect��

#include "define.h"
#include "flash.h"
#include "astar.h"

class CMover
{
protected:
	IMAGE* p_background;		// ����ͼƬ��ָ��
	IMAGE back;					// Ϊ�˼ӿ�clear���ٶȣ�����back��Ա����ÿ�εı���ͼ
	CFlashGroup flash_group;	// �����򶯻����е�����

	CRect rect;					// ��ǰ��λ��

	int* map;					// ��ͼ��ָ��

	int speed;

	int new_dir;				// �µ��˶����򣬼����������Ϊ�˸�����Ϸ�ָ�
	int dir;					// �˶�����


public:
	CMover() :p_background(NULL), map(NULL), speed(0), new_dir(DIR_NONE), dir(DIR_NONE) {}

	// ͨ��һ��ͼƬ��ʼ��mover����
	void init(IMAGE* p_back, IMAGE* p_face, CPoint mn, CRect r, int* mp, int s)
	{
		// �����ٶ�
		speed = s;

		// ���õ�ͼ��ַ
		map = mp;

		// ���õ�ǰλ��
		rect = r;

		// ���ñ���ͼָ��
		p_background = p_back;

		// ����ͼƬ�����ĸ�����Ķ�������
		int h = rect.shape.x;
		int w = rect.shape.y;

		int m = mn.x;	// ����  4  4
		int n = mn.y;	// ����  3  2

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

	// ����/��ȡ ��ǰλ�õ����Ͻ�����
	void SetSite(CPoint s) { rect.site = s; }
	CPoint GetSite() { return rect.site; }

	// ��ȡ ��ǰλ�õ����ĵ�����
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
		case DIR_DOWN:  rect.site.x += s; break;	// ��
		case DIR_RIGHT: rect.site.y += s; break;	// ��
		case DIR_UP:	rect.site.x -= s; break;	// ��
		case DIR_LEFT:	rect.site.y -= s; break;	// ��
		}
	}

	void SetNewDir(int d)
	{
		new_dir = d;
	}

	void GetNearCross(int& dx, int& dy, int& i, int& j)
	{
		CPoint c = GetCenter();

		// ��������ĸ��ľ���
		dx = (c.x + BLOCK_SIZE / 2) % BLOCK_SIZE - BLOCK_SIZE / 2;
		dy = (c.y + BLOCK_SIZE / 2) % BLOCK_SIZE - BLOCK_SIZE / 2;

		int x = c.x - dx;
		int y = c.y - dy;

		i = x / BLOCK_SIZE;
		j = y / BLOCK_SIZE;
	}

	bool Turn()
	{
		// ת���ж�
		// ����Ҫ��Զ������һ��������λ�ڹ̶��Ĺ���ϣ����������ƶ�����������꣬
		// �����ת���ʱ����Ҫ�����ϸ�����


		// �Ƿ���ת��
			// �Ƿ���ת�������ڣ�
				// ת����Ƿ�ײǽ
					// ��ͨ������ȷת�䣬���·���ǰ���������¶�������
				// ײǽ�ˣ�����ת�䣬���ɷ���ǰ��
			// ����ת�������ڣ�����ת�䣬���ɷ���ǰ��
		// û��ת�򣬰��ɷ���ǰ��

		if (abs(new_dir) * abs(dir) == 2)	// �Ƿ���ת��
		{
			// ת������
			const int t = MOVE_TOLERENCE;	// ��������

			int dx, dy, i, j;
			GetNearCross(dx, dy, i, j);

			bool turn_flag = 0;

			if ((abs(new_dir) == 2 && abs(dx) < t) || (abs(new_dir) == 1 && abs(dy) < t))
				turn_flag = 1;

			if (turn_flag)		// �Ƿ���ת��������
			{
				// ��һ�����
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

				if (!edge_flag)	// �Ƿ��ڱ߽�
				{
					bool wall_flag = 0;

					if (map[i * MAP_COLUMN + j] == 3)
						wall_flag = 1;

					if (!wall_flag)	// ת����Ƿ�ײǽ
					{
						// ����ֵ�����޸Ķ���

						dir = new_dir;	

						// ��������
						rect.site.x -= dx;
						rect.site.y -= dy;

						return 1;
					}
					else
					{
						return 0;
					}

				}
				else	// ��������߽磬����ת��
				{
					return 0;
				}

			}
			else	// ת���ײǽ������ת��
			{
				return 0;
			}

		}
		else	// �����ǵ�ͷ���ߴ�DIR_NONE��
		{
			dir = new_dir;
			return 1;
		}
	}

	bool Go()
	{
		// �������Լ�����Ľڵ㣬��֪���Լ���˽ڵ�����λ��
		// ���� �ƶ����� �� ��ڵ�����λ��, ��һ���ڵ������
		// �����Լ�����һ���ڵ�ľ���
		// �ж�����״̬
			// ����ײ��/����ǽ
			// ��ײ��������Ȼ��һ������
			// �Ѿ�ײ���ˣ�û�о���

		int dx, dy, i, j;
		GetNearCross(dx, dy, i, j);

		switch (dir)
		{
		case DIR_DOWN:  if (dx >= 0) i++; break;	// ��
		case DIR_RIGHT: if (dy >= 0) j++; break;	// ��
		case DIR_UP:	if (dx <= 0) i--; break;	// ��
		case DIR_LEFT:	if (dy <= 0) j--; break;	// ��
		}

		// ��������߽磬�Ͳ�����ײǽ���
		if (j < 0 || j >= MAP_COLUMN)
		{
			// �ƶ�
			Move(speed);

			if (rect.site.y < 0)
				rect.site.y += GAME_WIDTH;
			else if (rect.site.y >= GAME_WIDTH)
				rect.site.y -= GAME_WIDTH;

			return 1;
		}


		int s = speed;	// �ƶ�����

		if (map[i * MAP_COLUMN + j] == 3)	// ǽ
		{
			const int t = BLOCK_SIZE - PERSON_SIZE / 2;	// ǽ�ڵĺ��

			// ������ǽ�ľ���
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

		// �ƶ�
		Move(s);

		return 1;
	}

	void Update()
	{
		flash_group.Update();

		// ת��
		if (Turn())
			SetFlash(dir);

		// ǰ��
		Go();
	}

	void Draw()
	{
		// Ϊ�����ۣ�
		// ��ͼ����draw �� ʵ������site ֮����ڹ̶�ƫ�� bias = BLOCK_SIZE / 2
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
	int status;			// 0׷��1�־塢2����
	int mode;			// ��ͬmode׷��ģʽ��ͬ
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

		// ��ӿ־�/�����Ĺ��ﶯ��
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
				flash_group.Add(flash);
			}	
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

			if (cd >= 100)
				flash_group.SetIdx(4);
			else if (cd % 10 > 5)
				flash_group.SetIdx(5);
			else
				flash_group.SetIdx(4);
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
		else if (status == 2)
		{
			switch (d)
			{
			case DIR_DOWN:  flash_group.SetIdx(8);  break;
			case DIR_UP:	flash_group.SetIdx(6);  break;
			case DIR_RIGHT: flash_group.SetIdx(7);  break;
			case DIR_LEFT:	flash_group.SetIdx(9);  break;
			}
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
			// player��λ��
			int pdx, pdy, pi, pj;
			p->GetNearCross(pdx, pdy, pi, pj);

			// A* �㷨�������ƶ�����
			brain.init(map, CPoint(mi, mj), CPoint(pi, pj));
			brain.BuildAWay();

			int dir = brain.GetDir();
			SetNewDir(dir);
				
			// ת��
			if (Turn())
				SetFlash(dir);

			// ǰ��
			Go();
		}
		else if (status == 1)
		{
			// player��λ��
			int pdx, pdy, pi, pj;
			p->GetNearCross(pdx, pdy, pi, pj);


			// A* �㷨�������ƶ�����
			brain.init(map, CPoint(mi, mj), CPoint(pi, pj));
			brain.BuildAWay();

			int dir = brain.GetDir();

			const int dirs[4] = { DIR_UP,DIR_RIGHT,DIR_DOWN,DIR_LEFT };

			for (int i = 0; i < 4; i++)
			{
				if (dir == dirs[i])
					continue;
				SetNewDir(dirs[i]);

				// ת��
				if (Turn())
					SetFlash(dir);

				// ǰ��
				if (Go())
					break;
			}
		}
		else
		{
			// A* �㷨�������ƶ�����
			brain.init(map, CPoint(mi, mj), CPoint(14, 14));
			brain.BuildAWay();

			int dir = brain.GetDir();

			if (dir == DIR_NONE)
				Burn();
			else
				SetNewDir(dir);

			// ת��
			if (Turn())
				SetFlash(dir);

			// ǰ��
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

	int Eat()	// �Զ�
	{
		const int t = EAT_TOLERANCE;

		// �ڵ�λ��
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

	bool Fight(CMonster** mons_list, int num)		// �Թ���
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