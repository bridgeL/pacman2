//zh_CN.GBK
#include "game.h"

int cnt1 = 1;
int cnt2 = 1;

int game_mode;

bool game_close;				// 指导多线程关闭的全局变量
bool update_event;				// 指导绘图更新的全局变量，控制帧率
char key;						// 键盘符号暂存

CPacman pacman;			// 吃豆人	
CMonster red;			// 四个怪物
CMonster pink;
CMonster orange;
CMonster blue;
CMonster* mons_list[4] = {&red, &pink, &orange, &blue};

int map[MAP_CNT];		// 地图 31 x 28 行列式存储（但是需要注意 easyx 是 列行式绘图）

IMAGE background;		// 背景图片

int bean_cnt;
int win;
int highest_score;
int choose;

void HpSleep(int ms)
{
	static clock_t oldclock = clock();		// 静态变量，记录上一次 tick

	oldclock += ms * CLOCKS_PER_SEC / 1000;	// 更新 tick

	if (clock() > oldclock)					// 如果已经超时，无需延时
		oldclock = clock();
	else
		while (clock() < oldclock)			// 延时
			Sleep(1);						// 释放 CPU 控制权，降低 CPU 占用率
}

DWORD WINAPI time_thread(PVOID param)
{
	int fps = (int)param;
	int ms = 1000 / fps;
	while (1)
	{
		HpSleep(ms);
		update_event = 1;
		cnt1++;
	}

	return 0L;
}

DWORD WINAPI keyboard_thread(PVOID)
{
	while (1)
	{
		int a = _getch();
		if (a == 224)
		{
			a = _getch();
			switch (a)
			{
			case 72: key = 'w'; break;
			case 80: key = 's'; break;
			case 75: key = 'a'; break;
			case 77: key = 'd'; break;
			}
		}
		else if (a == 13)
		{
			key = ' ';
		}
		else
		{
			key = a;
		}
	}

	return 0L;
}

#define LOAD_PNG_FROM_RC(PIMAGE,PNG_ID,ROW,COLUMN);					\
	loadimage(PIMAGE, _T("PNG"), MAKEINTRESOURCE(PNG_ID),			\
			PERSON_SIZE * COLUMN, PERSON_SIZE * ROW, 1);

#define LOAD_MONSTER_FACE(PIMAGE, PNG_ID);							\
	LOAD_PNG_FROM_RC(PIMAGE, PNG_ID, 4, 2);

#define INIT_MONSTER(CMONSTER, PFACE, PDEAD, SPEED, BURN_R, BURN_C, INSTANCE, PATH, COLOR);		\
	CMONSTER.init_speed(SPEED);																	\
	CMONSTER.init_map(map);																		\
	CMONSTER.init_rect(CRect(BURN_R * BLOCK_SIZE - PERSON_SIZE / 2,								\
			BURN_C * BLOCK_SIZE - PERSON_SIZE / 2, PERSON_SIZE, PERSON_SIZE));					\
	CMONSTER.init_img(&background, PFACE, PDEAD);												\
/**CMONSTER.SwitchPathShow(COLOR);	/**/														\
	CMONSTER.SetBrainStyle(INSTANCE, PATH);

void init()
{
	// init variables
	game_close = 0;					// 指导游戏关闭	  全局变量
	update_event = 0;				// 指导绘图更新的全局变量，控制帧率
	key = 0;						// 键盘符号缓存

	srand((unsigned int)time(NULL));

	// init graph
	initgraph(GAME_WIDTH, GAME_HEIGHT + 20, EX_SHOWCONSOLE);		// The new version of EasyX will Rename EW_SHOWCONSOLE macro to EX_ SHOWCONSOLE

	// init pacman
	IMAGE img_pacman;
	LOAD_PNG_FROM_RC(&img_pacman, IDB_PNG1, 4, 3);

	pacman.init_speed(PACMAN_SPEED);
	pacman.init_map(map);
	pacman.init_rect(CRect(20 * BLOCK_SIZE - PERSON_SIZE / 2, 10 * BLOCK_SIZE - PERSON_SIZE / 2, PERSON_SIZE, PERSON_SIZE));
	pacman.init_img(&background, &img_pacman, 4, 3);

	// init monsters
	IMAGE img_dead, img_blue, img_pink, img_orange, img_red;

	LOAD_MONSTER_FACE(&img_blue, IDB_PNG2);
	LOAD_MONSTER_FACE(&img_orange, IDB_PNG3);
	LOAD_MONSTER_FACE(&img_pink, IDB_PNG4);
	LOAD_MONSTER_FACE(&img_red, IDB_PNG5);

	LOAD_PNG_FROM_RC(&img_dead, IDB_PNG6, 4, 2);

	INIT_MONSTER(blue, &img_blue, &img_dead, MONSTER_SPEED_1, 12, 10, 10, 1, BLUE);
	INIT_MONSTER(orange, &img_orange, &img_dead, MONSTER_SPEED_0, 13, 10, 3, 1, YELLOW);
	INIT_MONSTER(pink, &img_pink, &img_dead, MONSTER_SPEED_0, 12, 10, 1, 3, BROWN);
	INIT_MONSTER(red, &img_red, &img_dead, MONSTER_SPEED_1, 13, 10, 1, 10, RED);

	// 设置多线程
	CreateThread(NULL, 0, keyboard_thread, NULL, 0, NULL);
	CreateThread(NULL, 0, time_thread, (PVOID)FPS, 0, NULL);
}

void init_map()
{
	const static int a[MAP_CNT] =
	{
		3,3,3,3,3, 3,3,3,3,3, 3, 3,3,3,3,3, 3,3,3,3,3, 
		3,1,1,1,1, 1,1,1,1,1, 3, 1,1,1,1,1, 1,1,1,1,3,
		3,1,3,3,3, 1,3,3,3,1, 3, 1,3,3,3,1, 3,3,3,1,3,
		3,1,3,0,3, 1,3,0,3,1, 3, 1,3,0,3,1, 3,0,3,1,3,
		3,1,3,3,3, 1,3,3,3,1, 3, 1,3,3,3,1, 3,3,3,1,3,

		3,2,1,1,1, 1,1,1,1,1, 1, 1,1,1,1,1, 1,1,1,2,3,
		3,1,3,3,3, 1,3,1,3,3, 3, 3,3,1,3,1, 3,3,3,1,3,
		3,1,3,3,3, 1,3,1,3,3, 3, 3,3,1,3,1, 3,3,3,1,3,
		3,1,1,1,1, 1,3,1,1,1, 3, 1,1,1,3,1, 1,1,1,1,3,
		3,3,3,3,3, 1,3,3,3,1, 3, 1,3,3,3,1, 3,3,3,3,3,

		0,0,0,0,3, 1,3,0,0,0, 0, 0,0,0,3,1, 3,0,0,0,0,
		0,0,0,0,3, 1,3,0,3,3, 0, 3,3,0,3,1, 3,0,0,0,0,
		3,3,3,3,3, 1,3,0,3,0, 0, 0,3,0,3,1, 3,3,3,3,3,
		0,0,0,0,0, 1,0,0,3,0, 0, 0,3,0,0,1, 0,0,0,0,0,
		3,3,3,3,3, 1,3,0,3,3, 3, 3,3,0,3,1, 3,3,3,3,3,

		0,0,0,0,3, 1,3,0,0,0, 0, 0,0,0,3,1, 3,0,0,0,0,
		0,0,0,0,3, 1,3,0,3,3, 3, 3,3,0,3,1, 3,0,0,0,0,
		3,3,3,3,3, 1,3,0,3,3, 3, 3,3,0,3,1, 3,3,3,3,3,
		3,1,1,1,1, 1,1,1,1,1, 3, 1,1,1,1,1, 1,1,1,1,3,
		3,1,3,3,3, 1,3,3,3,1, 3, 1,3,3,3,1, 3,3,3,1,3,

		3,2,1,1,3, 1,1,1,1,1, 1, 1,1,1,1,1, 3,1,1,2,3,
		3,3,3,1,3, 1,3,1,3,3, 3, 3,3,1,3,1, 3,1,3,3,3,
		3,3,3,1,3, 1,3,1,3,3, 3, 3,3,1,3,1, 3,1,3,3,3,
		3,1,1,1,1, 1,3,1,1,1, 3, 1,1,1,3,1, 1,1,1,1,3,
		3,1,3,3,3, 3,3,3,3,1, 3, 1,3,3,3,3, 3,3,3,1,3,

		3,1,1,1,1, 1,1,1,1,1, 1, 1,1,1,1,1, 1,1,1,1,3,
		3,3,3,3,3, 3,3,3,3,3, 3, 3,3,3,3,3, 3,3,3,3,3,
	};

	for (int i = 0; i < MAP_CNT; i++)
		map[i] = a[i];
}

void init_background()
{
	IMAGE bb = IMAGE(GAME_WIDTH - 12, GAME_HEIGHT-6);
	background = IMAGE(GAME_WIDTH, GAME_HEIGHT);

	loadimage(&bb, _T("PNG"), MAKEINTRESOURCE(IDB_PNG7), GAME_WIDTH - 12 , GAME_HEIGHT-6, 1);

	SetWorkingImage(&background);

	putimage(6, 3, &bb);

	int bias = BLOCK_SIZE / 2;
	int s = BLOCK_SIZE;

	setlinecolor(BLUE);
	for (int i = 0; i < MAP_ROW; i++)
	{
		for (int j = 0; j < MAP_COLUMN; j++)
		{
			if (map[i * MAP_COLUMN + j] == 1)
			{
				setfillcolor(WHITE);
				solidcircle(j * s + bias, i * s + bias, POINT_SIZE);
			}
			else if (map[i * MAP_COLUMN + j] == 2)
			{
				setfillcolor(YELLOW);
				solidcircle(j * s + bias, i * s + bias, POINT_BIG_SIZE);
			}
		}
	}

	SetWorkingImage(NULL);
}

void set_game_mode(int mode)
{
	game_mode = mode;
	switch (mode)
	{
	case GAMING: gaming_page_init();	break;
	case END:	 end_page_init();		break;
	case MENU:	 menu_page_init();		break;
	}
}
 
//****************** page init ****************//

void menu_page_init()
{
	choose = 0;

	static const int w_base = BLOCK_SIZE * 5;
	static const int h_base = BLOCK_SIZE * 6;

	fillrectangle(w_base, h_base, w_base + 200, h_base + 200);

	settextcolor(BLACK);
	setbkcolor(WHITE);

	outtextxy(w_base + 30, h_base + 50, _T("GAME PACMAN"));
	outtextxy(w_base + 50, h_base + 80, _T("Play!"));
	outtextxy(w_base + 50, h_base + 110, _T("Exit"));
	outtextxy(w_base + 30, h_base + 140, _T("Highest Score"));

	TCHAR str[10];
	_itot_s(highest_score, str, 10);
	outtextxy(w_base + 140, h_base + 140, str);

	graphdefaults();

	FlushBatchDraw();
}

void end_page_init()
{
	choose = 0;

	static const int w_base = BLOCK_SIZE * 5;
	static const int h_base = BLOCK_SIZE * 6;
	fillrectangle(w_base, h_base, w_base + 200, h_base + 200);

	settextcolor(BLACK);
	setbkcolor(WHITE);

	if (win == 1)
		outtextxy(w_base + 30, h_base + 50, _T("You Win!"));
	else
		outtextxy(w_base + 30, h_base + 50, _T("You Lose!"));

	outtextxy(w_base + 30, h_base + 80, _T("Score:"));
	outtextxy(w_base + 50, h_base + 120, _T("Replay"));
	outtextxy(w_base + 50, h_base + 150, _T("Exit"));

	TCHAR str[10];
	_itot_s(pacman.GetScore(), str, 10);
	outtextxy(w_base + 110, h_base + 80, str);

	graphdefaults();

	FlushBatchDraw();
}

void gaming_page_init()
{
	bean_cnt = 0;					// 吃豆计数器，当全部豆子都被吃掉后，游戏结束
	win = 0;						// 游戏胜利/失败标志

	pacman.Reset();
	for (int i = 0; i < 4; i++)
		mons_list[i]->Reset();

	// init map
	init_map();

	// init background
	init_background();

	// UI print
	putimage(0, 0, &background);
	outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 10 * BLOCK_SIZE , _T("score:"));

	pacman.Draw();
	for (int i = 0; i < 4; i++)
		mons_list[i]->Draw();

	FlushBatchDraw();
}

//****************** page content ****************//

void menu_page()
{
	static const int w_base = BLOCK_SIZE * 5;
	static const int h_base = BLOCK_SIZE * 6;

	
	settextcolor(WHITE);

	if (choose == 0)
	{
		setbkcolor(BLACK);
		outtextxy(w_base + 20, h_base + 80, _T(">>"));
		setbkcolor(WHITE);
		outtextxy(w_base + 20, h_base + 110, _T("    "));
	}
	else
	{
		setbkcolor(WHITE);
		outtextxy(w_base + 20, h_base + 80, _T("    "));
		setbkcolor(BLACK);
		outtextxy(w_base + 20, h_base + 110, _T(">>"));
	}

	graphdefaults();

	FlushBatchDraw();
}

void gaming_page()
{
	// 清除
	pacman.Clear();
	for (int i = 0; i < 4; i++)
		mons_list[i]->Clear();

	for (int i = 0; i < 4; i++)
	{
		if(mons_list[i]->GetDir() == 0)	// Detect if any monsters are stuck due to dir=0
		{
			mons_list[i]->BugReset();		// Correct incorrect dir values
		}
	}

	// 坐标更新
	pacman.Update();
	for (int i = 0; i < 4; i++)
		mons_list[i]->Update(&pacman);

	// 吃豆子
	int bean_type = pacman.Eat();

	if (bean_type > 0)
	{
		// 吃完所有豆子，游戏胜利
		bean_cnt++;
		if (bean_cnt >= BEAN_NUM)
		{
			win = 1;
		}

		// 吃到大力丸
		if (bean_type == 2)
		{
			for (int i = 0; i < 4; i++)
				mons_list[i]->Fear(MONSTER_FEAR_TIME);
		}
	}

	// 与遇到的所有怪物战斗
	bool die = pacman.Fight(mons_list, 4);

	if (die)
	{
		win = -1;
	}

	// 绘图
	pacman.Draw();
	for (int i = 0; i < 4; i++)
		mons_list[i]->Draw();

	// 刷新显存
	FlushBatchDraw();

	if (win)
	{
		int scr = pacman.GetScore();
		if (scr > highest_score)
			highest_score = scr;
		set_game_mode(END);
	}
}

void end_page()
{
	static const int w_base = BLOCK_SIZE * 5;
	static const int h_base = BLOCK_SIZE * 6;

	settextcolor(WHITE);
	if (choose == 0)
	{
		setbkcolor(BLACK);
		outtextxy(w_base + 20, h_base + 120, _T(">>"));
		setbkcolor(WHITE);
		outtextxy(w_base + 20, h_base + 150, _T("    "));
	}
	else
	{
		setbkcolor(WHITE);
		outtextxy(w_base + 20, h_base + 120, _T("    "));
		setbkcolor(BLACK);
		outtextxy(w_base + 20, h_base + 150, _T(">>"));
	}

	graphdefaults();

	FlushBatchDraw();
}

//****************** page deal ****************//

void gaming_deal()
{
	// 设置参数
	if (key == 27)
		set_game_mode(END);

	switch (key)
	{
	case 'w': pacman.SetNewDir(DIR_UP); break;
	case 's': pacman.SetNewDir(DIR_DOWN); break;
	case 'a': pacman.SetNewDir(DIR_LEFT); break;
	case 'd': pacman.SetNewDir(DIR_RIGHT); break;
	}
}

void menu_deal()
{
	if (key == ' ' || key == 'z')
	{
		if (choose == 0)
			set_game_mode(GAMING);
		else
			game_close = 1;
	}
	else if (key == 'w')
	{
		choose--;
		if (choose < 0)
			choose = 0;
	}
	else if (key == 's')
	{
		choose++;
		if (choose >= 1)
			choose = 1;
	}
}

void end_deal()
{
	if (key == ' ' || key == 'z')
	{
		if (choose == 0)
			set_game_mode(MENU);
		else
			game_close = 1;
	}
	else if (key == 'w')
	{
		choose--;
		if (choose < 0)
			choose = 0;
	}
	else if (key == 's')
	{
		choose++;
		if (choose >= 1)
			choose = 1;
	}
}