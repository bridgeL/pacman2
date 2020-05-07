//zh_CN.GBK
#pragma once
// ���ĵ���Ҫ�����˶������� flash�࣬�Լ�����������еĹ����� flash_group��

#include "define.h"

class CFlash
{
private:
	vector<IMAGE*> m_list;
	size_t idx_now;				// ��ǰ��ʾ����

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

	// ͨ��update�����Ĳ��ϸ��£��л�����������չʾ��ͼƬ��ʵ�ֶ����ı仯
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
	int idx_now;	// �л���ͬ�Ķ������У�ʵ�ֲ�ͬ�Ķ���Ч��
	int cnt;		// ��ʱ�������ƶ��������ٶ�
	int top;

public:
	CFlashGroup(int t) :idx_now(0), cnt(0), top(t) {}
	CFlashGroup() :CFlashGroup(5) {}

	void Add(CFlash flash)
	{
		m_list.push_back(flash);
	}

	// ���õ�ǰչʾ�Ķ�������
	void SetIdx(int idx)
	{
		idx_now = idx;
	}

	IMAGE& GetImage()
	{
		return m_list[idx_now].GetImage();
	}

	// update flash����
	void Update()
	{
		cnt++;
		if (cnt >= top)
		{
			cnt = 0;
			m_list[idx_now].Update();

			// ȫ�����¡� ���� �������µ�ǰ��ʾ�Ķ�������

			//for (vector<CFlash>::iterator iter = m_list.begin(); iter != m_list.end(); iter++)
			//	(*iter).Update();
		}
	}
};