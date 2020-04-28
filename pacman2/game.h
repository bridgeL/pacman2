//zh_CN.GBK
#pragma once
#include "define.h"
#include "mover.h"
#include "resource.h"

extern int game_mode;

extern bool game_close;					// ָ����Ϸ�رյ�ȫ�ֱ���
extern bool update_event;				// ָ����ͼ���µ�ȫ�ֱ���������֡��
extern char key;						// ���̷����ݴ�

DWORD WINAPI time_thread(PVOID param);		// ��ʱ������߳�
DWORD WINAPI keyboard_thread(PVOID);		// ���̲�����߳�

void init();
void init_map();
void init_background();
void init_pacman();
void init_monster();

void gaming_page_init();
void menu_page_init();
void end_page_init();

void gaming_page();
void menu_page();
void end_page();

void gaming_deal();
void end_deal();
void menu_deal();

void set_game_mode(int mode);