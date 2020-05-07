//zh_CN.GBK
#include "game.h"

int main()
{
	// init
	init();

	BeginBatchDraw();

	set_game_mode(MENU);

	// 主循环xu
	while (!game_close)
	{
		if (key)
		{
			switch (game_mode)
			{
			case GAMING:	gaming_deal();		break;
			case END:		end_deal();			break;
			case MENU:		menu_deal();		break;
			}
			

			key = 0;
		}

		if (update_event)
		{
			update_event = 0;
			
			if (cnt1)
			{
				cnt2++;
				// 调试用的，用于检测是否存在丢帧问题
			}

			switch (game_mode)
			{
			case GAMING:	gaming_page();		break;
			case END:		end_page();			break;
			case MENU:		menu_page();		break;
			}

		}

	}
	
	EndBatchDraw();

	// 游戏退出

	closegraph();

	return 0;
}
