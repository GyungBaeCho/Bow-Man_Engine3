#pragma once

/*
	--참고사항--
	
	본 프레임워크는, Render와 Update를 병렬 처리할 수 있도록 구현되었다.
	병렬 처리모드와 일반 모드를 On/Off할 수 있도록 구현하였다.

	Render와 Update를 동시에 처리할 때 발생하는 Data Race 문제를 피하기 위하여,
	병렬모드일 때에는 1Frame 이전의 World를 Rendering한다.

	병렬모드 Update시에는 World Update와 다음 Frame Render에 사용될 Data를 생성하는 작업을 진행한다.
	이때 Update와 Render 프로세스에서 참조할 데이터를 구분하기 위하여

	CParallelPivot 객체를 정의하여 사용하도록 하였다.
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
