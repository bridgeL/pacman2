//zh_CN.GBK
#pragma once
#include "define.h"
#include "mover.h"
#include "resource.h"


extern int cnt1;
extern int cnt2;

extern int game_mode;

extern bool game_close;					// 指导游戏关闭的全局变量
extern bool update_event;				// 指导绘图更新的全局变量，控制帧率
extern char key;						// 键盘符号暂存

DWORD WINAPI time_thread(PVOID param);		// 定时捕获多线程
DWORD WINAPI keyboard_thread(PVOID);		// 键盘捕获多线程

void init();
void init_map();
void init_background();


void menu_page_init();
void gaming_page_init();
void end_page_init();

void menu_page();
void gaming_page();
void end_page();

void menu_deal();
void gaming_deal();
void end_deal();


void set_game_mode(int mode);