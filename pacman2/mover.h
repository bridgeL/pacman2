//zh_CN.GBK
#pragma once
// ���ĵ���Ҫ������ mover�࣬�Լ��������� monster��

#include "define.h"
#include "flash.h"
#include "astar.h"

// ���øÿ����ʹ�� TransparentBlt ����
#pragma comment( lib, "MSIMG32.LIB")

class CMover
{
protected:
	IMAGE* p_background;		// ����ͼƬ��ָ��
	IMAGE back;					// Ϊ�˼ӿ�clear���ٶȣ�����back��Ա����ÿ�εı���ͼ
	CFlashGroup faces;	// �����򶯻����е�����

	CRect rect;					// ��ǰ��λ��

	CPoint burn;				// �����ص�

	int* map;					// ��ͼ��ָ��

	double speed;
	double speed_d;

	int new_dir;				// �µ��˶����򣬼����������Ϊ�˸�����Ϸ�ָ�
	int dir;					// �˶�����


public:
	CMover() :p_background(NULL), map(NULL), speed(0), speed_d(0), new_dir(DIR_NONE), dir(DIR_NONE) 
	{
		faces = CFlashGroup(EAT_FLASH_TIME);
	}

	// ����ʼ���ֽ�Ϊһϵ�к���,�밴��˳��ִ��
	void init_speed(double s)
	{
		// �����ٶ�
		speed = s;
	}

	void init_map(int* m)
	{
		// ���õ�ͼ��ַ
		map = m;
	}

	void init_rect(CRect r)
	{
		// ���õ�ǰλ��
		rect = r;
		burn = rect.site;
	}

	void init_img(IMAGE* p_back, IMAGE* p_face, int r, int c)
	{
		// ���ñ���ͼָ��
		p_background = p_back;

		// ����ͼƬ�����ĸ�����Ķ�������
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

	// ����/��ȡ ��ǰλ�õ����Ͻ�����
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

		if (new_dir == DIR_NONE)
		{
			dir = DIR_NONE;
			return 1;
		}
		else if (abs(new_dir) != abs(dir) )	// �Ƿ���ת��
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


		int s = (int)speed;	// �ƶ�����

		if (map[i * MAP_COLUMN + j] == 3)	// ǽ
		{
			const int t = BLOCK_SIZE - PERSON_SIZE / 2;	// ǽ�ڵĺ��

			// ������ǽ�ľ���
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

		// �ƶ�
		Move(s);

		return 1;
	}

	void Update()
	{
		faces.Update();

		// ת��
		if (Turn())
			SetFlash(dir);

		// ǰ��
		Go();
	}

private:
	// ͸����ͼ����
	// ������
	//		dstimg: Ŀ�� IMAGE ����ָ�롣NULL ��ʾĬ�ϴ���
	//		x, y:	Ŀ����ͼλ��
	//		srcimg: Դ IMAGE ����ָ�롣NULL ��ʾĬ�ϴ���
	//		transparentcolor: ͸��ɫ��srcimg �ĸ���ɫ�����Ḵ�Ƶ� dstimg �ϣ��Ӷ�ʵ��͸����ͼ
	void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg, UINT transparentcolor)
	{
		HDC dstDC = GetImageHDC(dstimg);
		HDC srcDC = GetImageHDC(srcimg);
		int w = srcimg->getwidth();
		int h = srcimg->getheight();

		// ʹ�� Windows GDI ����ʵ��͸��λͼ
		TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, transparentcolor);
	}

public:
	void Draw()
	{
		// Ϊ�����ۣ�
		// ��ͼ����draw �� ʵ������site ֮����ڹ̶�ƫ�� bias = BLOCK_SIZE / 2
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
	int status;			// 0׷��1�־塢2����
	CPoint escape_rc;	// ���ܵĵص�
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
		// ����׷��ģʽ
		int instance = chase_mode.x;
		int path = chase_mode.y;
		brain.SetStyle(instance, path);
	}

	void init_img(IMAGE* p_back, IMAGE* p_face, IMAGE* p_dead)
	{
		// ���ñ���ͼָ��
		p_background = p_back;

		int h = rect.shape.x;
		int w = rect.shape.y;

		// ����ͼƬ�����ĸ�����Ķ�������
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

		// ��ӿ־�/�����Ĺ��ﶯ��
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

		// monster��λ��
		int mdx, mdy, mi, mj;
		GetNearCross(mdx, mdy, mi, mj);

		// player��λ��
		int pdx, pdy, pi, pj;
		p->GetNearCross(pdx, pdy, pi, pj);

		if (status == 0 )
		{
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


			// A* �㷨�������ƶ�����
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

			// ת��
			if (Turn())
				SetFlash(dir);

			// ǰ��
			Go();
		}
		else
		{
			// A* �㷨�������ƶ�����
			brain.init(map, CPoint(mi, mj), CPoint(13, 10));
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

	}

	void Reset()
	{
		CMover::Reset();
		cd = 0;
		status = 0;
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

	bool Fight(CMonster** mons_list, int num)		// �Թ���
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