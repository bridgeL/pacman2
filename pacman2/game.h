//zh_CN.GBK
#pragma once
#include "define.h"
#include "mover.h"
#include "resource.h"


extern int cnt1;
extern int cnt2;

extern int game_mode;

extern bool game_close;					// ָ����Ϸ�رյ�ȫ�ֱ���
extern bool update_event;				// ָ����ͼ���µ�ȫ�ֱ���������֡��
extern char key;						// ���̷����ݴ�

DWORD WINAPI time_thread(PVOID param);		// ��ʱ������߳�
DWORD WINAPI keyboard_thread(PVOID);		// ���̲�����߳�

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