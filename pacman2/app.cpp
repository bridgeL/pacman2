//zh_CN.GBK
#include "page.h"

int main()
{
	CGame game;
	// init
	game.Init();

	// 主循环xu
	while (!game_close)
	{
		game.Deal();
	}
	
	EndBatchDraw();

	// 游戏退出

	closegraph();

	return 0;
}
