//zh_CN.GBK
#pragma once
#include "define.h"

class CAStarNode
{
public:
	int father;
	int G;
	int H;
	bool close;
	bool open;

	CAStarNode() : father(0), G(0), H(0), close(0), open(0) {}
};


class CAStar
{
private:
	CAStarNode ns[MAP_CNT];	// �㼯

	list<int> open_list;		// �����б�
	//list<int> close_list;		// �ر��б� // ��ά��ͼ�ر��б�Ĵ�С��n^2������������ʱ���ߣ����ʹ�÷ֲ�ʽ�洢��ֱ�ӷ��ʽ��
								// �������б����������С������ʹ��list��ʽ����ʱ���Ǻܶ�

	int s;	// ��ʼ��㣨���
	int e;	// ������㣨��ң�

	int w_inst;	// ����Ȩ�أ���ֵԽ��monsterԽע�ؾ������
	int w_path;	// ·��Ȩ�أ���ֵԽ��monsterԽע�ض�·��

	COLORREF color;
	bool path_show;
	int cnt;

public:
	CAStar() :s(0), e(0), w_inst(1), w_path(1), path_show(0), color(RED), cnt(0) {}

	void SwitchPathShow(COLORREF c)
	{
		if (path_show)
			path_show = 0;
		else
			path_show = 1;

		color = c;
	}

	void SetStyle(int instance, int path)
	{
		w_inst = instance;
		w_path = path;
	}

	// ��ʼ��
	void init(int* map, CPoint sp, CPoint ep)
	{
		open_list.clear();
		//close_list.clear();
		
		e = ep.x * MAP_COLUMN + ep.y;
		s = sp.x * MAP_COLUMN + sp.y;

		for (int i = 0; i < MAP_CNT; i++)
		{
			//if (map[i] == 3)
				//close_list.push_back(i);
			ns[i].close = (map[i] == 3) ? 1 : 0;
			ns[i].open = 0;
			ns[i].G = 0;
			
			int dk = abs(i - e);
			ns[i].H = (dk / MAP_COLUMN + dk % MAP_COLUMN) * w_inst;

			ns[i].father = 0;
		}
	}

	// ��С��������������������������д������shit�ĳ���Ա����

	/**
	// ��С��ȡ����ɾ������Ԫ��
	int open_list_pop()
	{
		int max = open_list.size() - 1;
		int k = open_list[0];

		open_list[0] = open_list[max];	// βԪ�ػ�λ�����������Ƕ�����Ĺ���Ŀ���Ǳ��� ������ �� ��ȫ������ ����
		
		int t = 0;	
		int tk = open_list[0];
		int F = GetF(tk);

		while (1)			// �ھ���λ��ʣ�µ����𼶱���ȷ���Լ���λ�� // �������� ����һ�飬��С����ȥ����
		{
			int b1 = t * 2 + 1;	
			int b2 = t * 2 + 2;

			if (b1 >= max)	// ������Χ �� ��ʣһ�ˣ���������
				break;

			int b1k = open_list[b1];
			int b2k = open_list[b2];

			int F1 = GetF(b1k);
			int F2 = GetF(b2k);

			if ( F1 < F && F1 < F2 )	// ������˭С˭����
			{
				open_list[t] = b1k;
				open_list[b1] = tk;
				t = b1;
				tk = b1k;
				F = F1;
			}
			else if(F2 < F && F2 < F1)
			{
				open_list[t] = b2k;
				open_list[b2] = tk;
				t = b2;
				tk = b2k;
				F = F2;
			}
			else		// Ҳ����������λ��β��Ԫ���뿪������֮�飬��˻���˽ϸߵ����Σ�û�����³��صײ�
			{
				break;
			}
		}

		open_list.erase(open_list.end() - 1);	// ɾ����λ�Ĺھ���ϯλ

		return k;
	}

	// ��������С�ѵ�ĳһԪ�ص�λ��, ����ʵ�������жϣ��˴�һ�������ϵ�������ΪFֵ��С��Ԫ�زŻ�ִ�д˺���
	void open_list_adjust(int idx)
	{
		int b = idx;	// ��ǰԪ������

		while (1)	// ����Ԫ�ؿ�ʼ��֣�Ŀ���ǹھ�
		{
			if (b == 0)
				break;

			int t = (b + 1) / 2 - 1;		// �����ϲ�Ԫ������

			int bk = open_list[b];
			int tk = open_list[t];

			if (GetF(tk) > GetF(bk))	// ������������ϲ�Ԫ�����˾����²�Ԫ�ؽ�λ
			{
				open_list[t] = bk;
				open_list[b] = tk;
				b = t;
			}
			else								// ���������ˣ��²�Ԫ�ؾͽ������ĳ��֮��
			{
				break;
			}
		}
	}

	// ��С�� ����Ԫ�� ������λ��
	void open_list_add(int k)
	{
		open_list.push_back(k);

		int b = open_list.size() - 1;	// ��ǰԪ������

		while (1)	// ���²������Ԫ�ؿ�ʼ��֣�Ŀ���ǹھ�
		{
			if (b == 0)
				break;

			int t = (b + 1) / 2 - 1;		// �����ϲ�Ԫ������

			int bk = open_list[b];
			int tk = open_list[t];

			if (GetF(tk) > GetF(bk))	// ������������ϲ�Ԫ�����˾����²�Ԫ�ؽ�λ
			{
				open_list[t] = bk;
				open_list[b] = tk;
				b = t;
			}
			else								// ���������ˣ��²�Ԫ�ؾͽ������ĳ��֮��
			{
				break;
			}
		}
	}

	// ����һ��·��
	// ��open_list��ȡ��Fֵ��С�ĵ� current

	// ���������ܵ�û��close�ĵ� next
	// ���û��open�����ͼ����Gֵ��Ȼ����ӽ�open_list������Fֵ����λ��

	// ���open�����Ͳ鿴���ͨ���˵� current ���� next�Ƿ������
	// ��������㣬���·���
	// ������㣬�����¼�����Gֵ��ͬʱ�����µ�Fֵ������������С����λ��

	void BuildAWay()
	{
		open_list_add(s); // ��ӵ�һ�����
		ns[s].open = 1;

		int k_current = s;	// �������
		
		while (1)
		{
			if (k_current == e)
				break;
			
			if (open_list.size() == 0)
				break;

			// ��ѯ��һ�����
			k_current = open_list_pop();
			ns[k_current].close = 1;


			//setfillcolor(YELLOW);
			//solidcircle(k_current % MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2, k_current / MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2, 5);
			//
			//cout << open_list.size() << " current = " << k_current <<  "(" << k_current / MAP_COLUMN << ","<< k_current%MAP_COLUMN << ")" <<  endl;
			//_getch();
			//

			//setfillcolor(BLUE);
			//solidcircle(k_current % MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2, k_current / MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2, 5);

			// ��Ѱ��Χ���
			for (int i = 0; i < 4; i++)
			{
				int k_next = k_current;
				CPoint xy = CPoint(k_next / MAP_COLUMN, k_next % MAP_COLUMN);
				switch (i)
				{
				case 0: xy.x--; break;
				case 1: xy.x++; break;
				case 2: xy.y--; break;
				case 3: xy.y++; break;
				}

				if (xy.x < 0 || xy.x >= MAP_ROW || xy.y < 0 || xy.y >= MAP_COLUMN)
					continue;

				k_next = xy.x * MAP_COLUMN + xy.y;

				if (!ns[k_next].close)			// û����close �� ǽ������̽����ϵĽ�� ��
				{
					if (ns[k_next].open)		// �Ѿ�������open
					{
						if (ns[k_next].G > ns[k_current].G + 1)	// �Ż�·��
						{
							ns[k_next].G = ns[k_current].G + 1;
							ns[k_next].father = k_current;

							// F�����䶯�������Ҫ���µ�����С��
							for (int i = 0; i < open_list.size(); i++)
							{
								if (k_next == open_list[i])
								{
									open_list_adjust(i);
									break;
								}
							}
						}
					}
					else
					{
						ns[k_next].G = ns[k_current].G + 1;
						ns[k_next].father = k_current;

						open_list_add(k_next);			// ��ӽ�open_list
						ns[k_next].open = 1;

						//setfillcolor(WHITE);
						//solidcircle(k_next % MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2, k_next / MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2, 5);

					}
				}

			}
		}
	}
	/**/

	int FindMinF()
	{
		int k_min = 0;
		int F_min = 1000000;
		for (list<int>::iterator it = open_list.begin(); it != open_list.end();it++)
		{
			int k = (*it);
			int F = ns[k].G + ns[k].H;
			if (F < F_min)
			{
				F_min = F;
				k_min = k;
			}
		}

		if (k_min != 0)
		{
			open_list.remove(k_min);
		}

		return k_min;
	}

	void BuildAWay()
	{
		open_list.push_back(s); // ��ӵ�һ�����
		ns[s].open = 1;

		int k_current = s;	// �������

		while (1)
		{
			if (k_current == e)
				break;

			if (open_list.size() == 0)
				break;

			// ��ѯ��һ�����
			k_current = FindMinF();
			ns[k_current].close = 1;


			// ��Ѱ��Χ���
			for (int i = 0; i < 4; i++)
			{
				int ni = k_current / MAP_COLUMN;
				int nj = k_current % MAP_COLUMN;

				switch (i)
				{
				case 0: ni--; break;
				case 1: ni++; break;
				case 2: nj--; break;
				case 3: nj++; break;
				}

				if (ni < 0 || ni >= MAP_ROW || nj < 0 || nj >= MAP_COLUMN)
					continue;

				int k_next = ni * MAP_COLUMN + nj;

				if (!ns[k_next].close)			// û����close �� ǽ������̽����ϵĽ�� ��
				{
					int G_old = ns[k_next].G;
					int G_new = ns[k_current].G + w_path;

					if (ns[k_next].open)		// �Ѿ�������open
					{
						// �Ż�·��
						if (G_old > G_new )	
						{
							ns[k_next].G = G_new;
							ns[k_next].father = k_current;
						}
					}
					else
					{
						ns[k_next].G = G_new;
						ns[k_next].father = k_current;

						open_list.push_back(k_next);			// ��ӽ�open_list
						ns[k_next].open = 1;
					}
				}

			}
		}
	}


	// ��÷���
	int GetDir()
	{
		if (path_show)
		{
			cnt++;
			if (cnt > 5)
				cnt = 0;

			if (cnt == 0)
				setlinecolor(BLACK);
			else
				setlinecolor(color);
		}
		

		int k[2] = { 0 };
		
		k[0] = e;

		while (1)
		{
			k[1] = ns[k[0]].father;

			if (k[1] == 0 || k[1] == s)
				break;


			if (path_show)
			{
				int xi = k[0] / MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2;
				int xj = k[0] % MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2;
				int yi = k[1] / MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2;
				int yj = k[1] % MAP_COLUMN * BLOCK_SIZE + BLOCK_SIZE / 2;
				line(xj, xi, yj, yi);
			}


			k[0] = k[1];
	
		}

		if (k[0] - k[1] == MAP_COLUMN)	return DIR_DOWN;
		if (k[0] - k[1] == -MAP_COLUMN)	return DIR_UP;
		if (k[0] - k[1] == -1)			return DIR_LEFT;
		if (k[0] - k[1] == 1)			return DIR_RIGHT;

		return DIR_NONE;
	}



};

/**/