#pragma once
#include "define.h"
#include "mover.h"
#include "resource.h"

// 目前代码内存泄漏

#define LOAD_MONSTER_FACE_IMG(IMG_NAME,IMG_ID)					\
	IMAGE IMG_NAME;												\
	loadimage(&IMG_NAME, _T("PNG"), MAKEINTRESOURCE(IMG_ID),	\
				PERSON_SIZE * 2, PERSON_SIZE * 4, 1);

#define INIT_MONSTER(CMONSTER, FACE_IMG, DEAD_IMG, CHASE_MODE, SPEED)	\
	CMONSTER.init(&background, &FACE_IMG, &DEAD_IMG, CPoint(4, 2),		\
	CRect(0, 0, PERSON_SIZE, PERSON_SIZE),	map, SPEED, CHASE_MODE);

#define MONSTER_STYLE(CMONSTER, INSTANCE, PATH, COLOR)	\
	CMONSTER.SwitchPathShow(COLOR);						\
	CMONSTER.SwitchPathShow(COLOR);						\
	CMONSTER.SetBrainStyle(INSTANCE,PATH);

#define SET_MOVER_SITE(PMOVER_NAME,X,Y)									\
		PMOVER_NAME->SetSite(	CPoint(	X * BLOCK_SIZE - PERSON_SIZE / 2,	\
									Y * BLOCK_SIZE - PERSON_SIZE / 2));


bool game_close;				// 指导多线程关闭的全局变量
bool update_event;				// 指导绘图更新的全局变量，控制帧率
char key;						// 键盘符号暂存
int highest_score;
int win;


DWORD WINAPI time_thread(PVOID param)
{
	int fps = (int)param;
	int ms = 1000 / fps;
	while (1)
	{
		Sleep(ms);
		update_event = 1;
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


class IPage
{
protected:
	bool open = 0;
	int next_page = 0;

	virtual void SubInit() = 0;

public:
	void Init()
	{
		open = 1;
		next_page = 0;
		SubInit();
	}

	virtual void KeyDeal(char key) = 0;
	virtual void Update() = 0;

	bool IsOpen() { return open; }
	int GetNextPage() { return next_page; }
};


class CMenuPage :public IPage
{
private:
	int choose = 0;
	int w_base = 0;
	int h_base = 0;

	void SubInit()
	{
		choose = 0;

		w_base = BLOCK_SIZE * 5;
		h_base = BLOCK_SIZE * 6;

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

public:

	void KeyDeal(char key)
	{
		if (key == ' ' || key == 'z')
		{
			if (choose == 0)
			{
				next_page = GAMING;
				open = 0;
			}
			else
			{
				next_page = EXIT;
				open = 0;
			}
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

	void Update()
	{
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
};

class CGamingPage :public IPage
{
private:
	int bean_cnt;

	CPacman* pacman;			// 吃豆人
	CMonster* red;			// 四个怪物
	CMonster* blue;
	CMonster* orange;
	CMonster* pink;

	int* map;		// 地图 31 x 28 行列式存储（但是需要注意 easyx 是 列行式绘图）

	IMAGE* background;		// 背景图片

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
		SetWorkingImage(background);
		clearrectangle(0, 0, GAME_WIDTH, GAME_WIDTH);
		SetWorkingImage(NULL);

		loadimage(background, _T("PNG"), MAKEINTRESOURCE(IDB_PNG7), GAME_WIDTH, GAME_HEIGHT, 1);

		SetWorkingImage(background);

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

	void SubInit()
	{
		SET_MOVER_SITE(pacman, 20, 10);
		pacman->Reset();

		SET_MOVER_SITE(blue, 12, 10);
		SET_MOVER_SITE(orange, 13, 10);
		SET_MOVER_SITE(pink, 12, 10);
		SET_MOVER_SITE(red, 13, 10);

		blue->Reset();
		orange->Reset();
		pink->Reset();
		red->Reset();


		init_map();
		init_background();

		bean_cnt = 0;					// 吃豆计数器，当全部豆子都被吃掉后，游戏结束
		win = 0;						// 游戏胜利/失败标志

		// UI print
		putimage(0, 0, background);
		outtextxy(1 * BLOCK_SIZE + BLOCK_SIZE / 2, 11 * BLOCK_SIZE + BLOCK_SIZE / 2, _T("score:"));

		pacman->Draw();
		blue->Draw();
		orange->Draw();
		pink->Draw();
		red->Draw();

		FlushBatchDraw();

	}

public:
	CGamingPage() :bean_cnt(0),pacman(NULL), red(NULL), blue(NULL), pink(NULL), orange(NULL), map(NULL), background(NULL) {};

	CGamingPage(CPacman* pac, CMonster* r, CMonster* b, CMonster* o, CMonster* p, int* m, IMAGE* back) :bean_cnt(0),pacman(pac), red(r), blue(b), pink(p), orange(o), map(m), background(back) {};

	void KeyDeal(char key)
	{
		// 设置参数
		if (key == 'q')
		{
			open = 0;
			next_page = END;
		}

		switch (key)
		{
		case 'w': pacman->SetNewDir(DIR_UP); break;
		case 's': pacman->SetNewDir(DIR_DOWN); break;
		case 'a': pacman->SetNewDir(DIR_LEFT); break;
		case 'd': pacman->SetNewDir(DIR_RIGHT); break;
		}
	}

	void Update()
	{
		CMonster* mons_list[4] = { red,blue,pink,orange };

		// 清除
		pacman->Clear();
		for (int i = 0; i < 4; i++)
			mons_list[i]->Clear();

		// 坐标更新
		pacman->Update();
		for (int i = 0; i < 4; i++)
			mons_list[i]->Update(pacman);

		// 吃豆子
		int bean_type = pacman->Eat();

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
					mons_list[i]->Fear(300);
			}
		}

		// 与遇到的所有怪物战斗
		bool die = pacman->Fight(mons_list, 4);

		if (die)
		{
			win = -1;
		}

		// 绘图
		pacman->Draw();
		for (int i = 0; i < 4; i++)
			mons_list[i]->Draw();

		// 刷新显存
		FlushBatchDraw();

		if (win)
		{
			int scr = pacman->GetScore();
			if (scr > highest_score)
				highest_score = scr;

			open = 0;
			next_page = END;
		}
	}
};

class CEndPage :public IPage
{
private:
	int choose = 0;
	int w_base = 0;
	int h_base = 0;

	void SubInit()
	{
		choose = 0;

		w_base = BLOCK_SIZE * 5;
		h_base = BLOCK_SIZE * 6;

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
		_itot_s(highest_score, str, 10);
		outtextxy(w_base + 110, h_base + 80, str);

		graphdefaults();

		FlushBatchDraw();
	}

public:

	void KeyDeal(char key)
	{
		if (key == ' ' || key == 'z')
		{
			if (choose == 0)
			{
				open = 0;
				next_page = MENU;
			}
			else
			{
				open = 0;
				next_page = EXIT;
			}
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

	void Update()
	{
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
};



class CGame
{
private:
	int current_page;
	CMenuPage menu_page;
	CGamingPage gaming_page;
	CEndPage end_page;

	CPacman pacman;			// 吃豆人
	CMonster red;			// 四个怪物
	CMonster blue;
	CMonster orange;
	CMonster pink;

	int *map;		// 地图 31 x 28 行列式存储（但是需要注意 easyx 是 列行式绘图）

	IMAGE background;		// 背景图片

	
	void init_pacman()
	{
		IMAGE img_pacman;
		loadimage(&img_pacman, _T("PNG"), MAKEINTRESOURCE(IDB_PNG1),
			PERSON_SIZE * 3, PERSON_SIZE * 4, 1);

		pacman.init(&background, &img_pacman, CPoint(4, 3),
			CRect(0, 0, PERSON_SIZE, PERSON_SIZE), map, PACMAN_SPEED);


	}

	void init_monster()
	{
		// blue orange	pink	red dead
		// 2	3		4		5	6

		IMAGE img_dead;
		loadimage(&img_dead, _T("PNG"), MAKEINTRESOURCE(IDB_PNG6), PERSON_SIZE * 2, PERSON_SIZE * 4, 1);

		// init blue
		LOAD_MONSTER_FACE_IMG(img_blue, IDB_PNG2);
		INIT_MONSTER(blue, img_blue, img_dead, 2, MONSTER_SPEED_1);
		MONSTER_STYLE(blue, 10, 1, BLUE);

		// init orange
		LOAD_MONSTER_FACE_IMG(img_orange, IDB_PNG3);
		INIT_MONSTER(orange, img_orange, img_dead, 3, MONSTER_SPEED_0);
		MONSTER_STYLE(orange, 3, 1, YELLOW);

		// init pink
		LOAD_MONSTER_FACE_IMG(img_pink, IDB_PNG4);
		INIT_MONSTER(pink, img_pink, img_dead, 1, MONSTER_SPEED_1);
		MONSTER_STYLE(pink, 1, 3, BROWN);

		// init red
		LOAD_MONSTER_FACE_IMG(img_red, IDB_PNG5);
		INIT_MONSTER(red, img_red, img_dead, 0, MONSTER_SPEED_0);
		MONSTER_STYLE(red, 1, 10, RED);
	}

public:

	CGame() :current_page(0), map(NULL) {};


	void Init()
	{
		// init variables
		game_close = 0;					// 指导游戏关闭	  全局变量
		update_event = 0;				// 指导绘图更新的全局变量，控制帧率
		key = 0;						// 键盘符号缓存
		highest_score = 0;

		map = new int[MAP_CNT];

		// init graph
		initgraph(GAME_WIDTH, GAME_HEIGHT, EW_SHOWCONSOLE);

		background = IMAGE(GAME_WIDTH, GAME_HEIGHT);

		init_pacman();
		init_monster();

		// 设置多线程
		CreateThread(NULL, 0, keyboard_thread, NULL, 0, NULL);
		CreateThread(NULL, 0, time_thread, (PVOID)30, 0, NULL);

		BeginBatchDraw();

		gaming_page = CGamingPage(&pacman, &red, &blue, &orange, &pink, map, &background);

		current_page = MENU;
		menu_page.Init();
	}

	void Deal()
	{
		if (key != 0)
		{
			switch (current_page)
			{
			case MENU:	 menu_page.KeyDeal(key);	break;
			case GAMING: gaming_page.KeyDeal(key);	break;
			case END:	 end_page.KeyDeal(key);		break;
			}
			key = 0;
		}

		if (update_event)
		{
			update_event = 0;
			switch (current_page)
			{
			case MENU:	 menu_page.Update();	break;
			case GAMING: gaming_page.Update();	break;
			case END:	 end_page.Update();		break;
			}
		}

		bool open = 0;
		
		switch (current_page)
		{
		case MENU:	 open = menu_page.IsOpen();			break;
		case GAMING: open = gaming_page.IsOpen();		break;
		case END:	 open = end_page.IsOpen();			break;
		}

		if (!open)
		{
			switch (current_page)
			{
			case MENU:	 current_page = menu_page.GetNextPage();		break;
			case GAMING: current_page = gaming_page.GetNextPage();		break;
			case END:	 current_page = end_page.GetNextPage();			break;
			}

			switch (current_page)
			{
			case MENU:	 menu_page.Init();		break;
			case GAMING: gaming_page.Init();	break;
			case END:	 end_page.Init();		break;
			case EXIT:	 game_close = 1;		break;
			}
		}
	}

};


