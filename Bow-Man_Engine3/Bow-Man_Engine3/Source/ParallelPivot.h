#pragma once

/*
	--�������--
	
	�� �����ӿ�ũ��, Render�� Update�� ���� ó���� �� �ֵ��� �����Ǿ���.
	���� ó������ �Ϲ� ��带 On/Off�� �� �ֵ��� �����Ͽ���.

	Render�� Update�� ���ÿ� ó���� �� �߻��ϴ� Data Race ������ ���ϱ� ���Ͽ�,
	���ĸ���� ������ 1Frame ������ World�� Rendering�Ѵ�.

	���ĸ�� Update�ÿ��� World Update�� ���� Frame Render�� ���� Data�� �����ϴ� �۾��� �����Ѵ�.
	�̶� Update�� Render ���μ������� ������ �����͸� �����ϱ� ���Ͽ�

	CParallelPivot ��ü�� �����Ͽ� ����ϵ��� �Ͽ���.
*/

class CParallelPivot {
public:
	CParallelPivot() {}
	~CParallelPivot() {}

	void Update() {
		if (m_bParallel) {
			m_pvtUpdate = 1 - m_pvtUpdate;
			m_pvtRender = 1 - m_pvtRender;
		}
	}
	void SetParallel(bool bFlag) {
		if (bFlag) {//Parallel On
			m_pvtUpdate = 0;
			m_pvtRender = 1;
		}
		else {		//Parallel Off
			m_pvtUpdate = 0;
			m_pvtRender = 0;
		}
		m_bParallel = bFlag;
	}
	int GetUpdatePivot() {
		return m_pvtUpdate;
	}
	int GetRenderPivot() {
		return m_pvtRender;
	}

private:
	 volatile int m_pvtUpdate	= 0;
	 volatile int m_pvtRender	= 0;
	 volatile bool m_bParallel	= false;
};

_declspec(selectany) CParallelPivot g_ParallelPivot;
#define PivotUpdate	g_ParallelPivot.Update
#define SetParallel(X) g_ParallelPivot.SetParallel(X)
#define UpdatePivot g_ParallelPivot.GetUpdatePivot()
#define RenderPivot g_ParallelPivot.GetRenderPivot()
