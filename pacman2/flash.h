//zh_CN.GBK
#pragma once
// 此文档主要定义了动画序列 flash类，以及多个动画序列的管理类 flash_group类

#include "define.h"

class CFlash
{
private:
	vector<IMAGE*> m_list;
	size_t idx_now;				// 当前显示索引

public:
	CFlash() :idx_now(0) {}

	void Add(IMAGE image)
	{
		IMAGE* p = new IMAGE(image);
		m_list.push_back(p);
	}

	IMAGE& GetImage()
	{
		return *(m_list[idx_now]);
	}

	// 通过update函数的不断更新，切换动画序列中展示的图片，实现动作的变化
	void Update()
	{
		idx_now++;
		if (idx_now >= m_list.size())
			idx_now = 0;
	}
};

class CFlashGroup
{
private:
	vector<CFlash> m_list;
	int idx_now;	// 切换不同的动画序列，实现不同的动作效果
	int cnt;		// 计时器，控制动画序列速度
	int top;

public:
	CFlashGroup(int t) :idx_now(0), cnt(0), top(t) {}
	CFlashGroup() :CFlashGroup(5) {}

	void Add(CFlash flash)
	{
		m_list.push_back(flash);
	}

	// 设置当前展示的动画序列
	void SetIdx(int idx)
	{
		idx_now = idx;
	}

	IMAGE& GetImage()
	{
		return m_list[idx_now].GetImage();
	}

	// update flash对象
	void Update()
	{
		cnt++;
		if (cnt >= top)
		{
			cnt = 0;
			m_list[idx_now].Update();

			// 全部更新↓ 或者 ↑仅更新当前显示的动画序列

			//for (vector<CFlash>::iterator iter = m_list.begin(); iter != m_list.end(); iter++)
			//	(*iter).Update();
		}
	}
};