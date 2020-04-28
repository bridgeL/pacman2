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
	CAStarNode ns[MAP_CNT];	// 点集

	list<int> open_list;		// 开放列表
	//list<int> close_list;		// 关闭列表 // 二维地图关闭列表的大小成n^2增长，遍历耗时极高，因此使用分布式存储，直接访问结果
								// 而开放列表的数量级较小，可以使用list形式，耗时不是很多

	int s;	// 开始结点（怪物）
	int e;	// 结束结点（玩家）

	int w_inst;	// 距离权重，此值越大，monster越注重距离控制
	int w_path;	// 路径权重，此值越大，monster越注重短路径

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

	// 初始化
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

	// 最小堆排序做出的垃圾，把它留给写下这坨shit的程序员看吧

	/**
	// 最小堆取出并删除顶部元素
	int open_list_pop()
	{
		int max = open_list.size() - 1;
		int k = open_list[0];

		open_list[0] = open_list[max];	// 尾元素换位至顶部，这是堆排序的规则，目的是保持 堆排序 的 完全二叉树 性质
		
		int t = 0;	
		int tk = open_list[0];
		int F = GetF(tk);

		while (1)			// 冠军退位，剩下的人逐级比赛确定自己的位置 // 比赛规则 三人一组，最小的人去顶层
		{
			int b1 = t * 2 + 1;	
			int b2 = t * 2 + 2;

			if (b1 >= max)	// 超出范围 或 仅剩一人，比赛结束
				break;

			int b1k = open_list[b1];
			int b2k = open_list[b2];

			int F1 = GetF(b1k);
			int F2 = GetF(b2k);

			if ( F1 < F && F1 < F2 )	// 比赛，谁小谁晋级
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
			else		// 也可能是新上位的尾部元素离开了死亡之组，因此获得了较高的名次，没有重新沉回底部
			{
				break;
			}
		}

		open_list.erase(open_list.end() - 1);	// 删除退位的冠军的席位

		return k;
	}

	// 仅调整最小堆的某一元素的位置, 根据实际情形判断，此处一定是向上调整，因为F值变小的元素才会执行此函数
	void open_list_adjust(int idx)
	{
		int b = idx;	// 当前元素索引

		while (1)	// 新增元素开始冲分，目标是冠军
		{
			if (b == 0)
				break;

			int t = (b + 1) / 2 - 1;		// 计算上层元素索引

			int bk = open_list[b];
			int tk = open_list[t];

			if (GetF(tk) > GetF(bk))	// 晋级赛，如果上层元素输了就让下层元素进位
			{
				open_list[t] = bk;
				open_list[b] = tk;
				b = t;
			}
			else								// 晋级赛输了，下层元素就结束它的冲分之旅
			{
				break;
			}
		}
	}

	// 最小堆 新增元素 并调整位置
	void open_list_add(int k)
	{
		open_list.push_back(k);

		int b = open_list.size() - 1;	// 当前元素索引

		while (1)	// 最下层的新增元素开始冲分，目标是冠军
		{
			if (b == 0)
				break;

			int t = (b + 1) / 2 - 1;		// 计算上层元素索引

			int bk = open_list[b];
			int tk = open_list[t];

			if (GetF(tk) > GetF(bk))	// 晋级赛，如果上层元素输了就让下层元素进位
			{
				open_list[t] = bk;
				open_list[b] = tk;
				b = t;
			}
			else								// 晋级赛输了，下层元素就结束它的冲分之旅
			{
				break;
			}
		}
	}

	// 构建一条路径
	// 从open_list中取出F值最小的点 current

	// 考察它四周的没有close的点 next
	// 如果没有open过，就计算好G值，然后添加进open_list，根据F值调整位置

	// 如果open过，就查看如果通过此点 current 到达 next是否更划算
	// 如果不划算，无事发生
	// 如果划算，就重新计算其G值，同时根据新的F值，调整它在最小堆中位置

	void BuildAWay()
	{
		open_list_add(s); // 添加第一个结点
		ns[s].open = 1;

		int k_current = s;	// 设置起点
		
		while (1)
		{
			if (k_current == e)
				break;
			
			if (open_list.size() == 0)
				break;

			// 查询下一个结点
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

			// 搜寻周围结点
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

				if (!ns[k_next].close)			// 没加入close （ 墙，或者探查完毕的结点 ）
				{
					if (ns[k_next].open)		// 已经加入了open
					{
						if (ns[k_next].G > ns[k_current].G + 1)	// 优化路径
						{
							ns[k_next].G = ns[k_current].G + 1;
							ns[k_next].father = k_current;

							// F发生变动，因此需要重新调整最小堆
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

						open_list_add(k_next);			// 添加进open_list
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
		open_list.push_back(s); // 添加第一个结点
		ns[s].open = 1;

		int k_current = s;	// 设置起点

		while (1)
		{
			if (k_current == e)
				break;

			if (open_list.size() == 0)
				break;

			// 查询下一个结点
			k_current = FindMinF();
			ns[k_current].close = 1;


			// 搜寻周围结点
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

				if (!ns[k_next].close)			// 没加入close （ 墙，或者探查完毕的结点 ）
				{
					int G_old = ns[k_next].G;
					int G_new = ns[k_current].G + w_path;

					if (ns[k_next].open)		// 已经加入了open
					{
						// 优化路径
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

						open_list.push_back(k_next);			// 添加进open_list
						ns[k_next].open = 1;
					}
				}

			}
		}
	}


	// 获得方向
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