#pragma once
#define TimerEngine CTimer::GetInstance()
#define TimeElapsed CTimer::GetInstance()->GetElapsedTime()

//Dummy Declare
class CUiObject;

class CTimer {
private:
	static CTimer* m_pInstance;

	// 실제 Advanced Time
	double		m_dAdvanceTime		= 0;
	// Time Lock이 유지된 시간
	double		m_dWaitTime			= 0;
	// Wait Time + Advance Time (Scene Update에 사용되는 최종 Elapsed Time)
	double		m_dDeltaTime		= -1;

	double		m_dSecondsPerCount	= 0;
	double		m_dSecondCounter	= 0;

	// Time Lock 시간
	double		m_dLockTime			= 1/70.0;

	int			m_nFrameCounter		= 0;

	double		m_dSumElapsedTime	= 0;
	double		m_dSumAdvanceTime	= 0;

	int			m_nAvgFPS			= 0;
	double		m_dAvgElapsedTime	= 0;
	double		m_dAvgAdvanceTime	= 0;

	__int64		m_nPrevTime			= 0;
	__int64		m_nCurrTime			= 0;

	WCHAR		m_wName[50];
	WCHAR		m_wcTimeInfo[50];
	WCHAR		m_wcAdvanceInfo[50];

private:
	CTimer() {}
	~CTimer() {
		Release();
	}

public:
	static void CreateInstance() { if (m_pInstance == nullptr) m_pInstance = new CTimer; }
	static CTimer* GetInstance() { return m_pInstance; }

	void CTimer::Initialize(int nLockFPS = 70);
	void Release() {}

	float Update();

	void SetName(WCHAR* pwName) {
		lstrcpy(m_wName, pwName);
	}
	float GetElapsedTime() {
		return (float)m_dDeltaTime;
	}
	float GetAvgWaitTime() {
		return static_cast<float>(m_dAvgAdvanceTime);
	}
	float GetAvgElapsedTime() {
		return (float)m_dAvgElapsedTime;
	}
	int GetAvgFPS() {
		return m_nAvgFPS;
	}
	WCHAR* GetText() {
		return m_wcTimeInfo;
	}
	WCHAR* GetAdvanceInfo() {
		return m_wcAdvanceInfo;
	}
};


__declspec(selectany) CTimer* CTimer::m_pInstance = nullptr;


class CStopWatch {
private:
	double		m_dSecondsPerCount;
	double		m_dDeltaTime;
	double		m_dSecondCounter;

	int			m_nFrameCounter;

	int			m_nAvgFPS;
	double		m_dSumElapsedTime;
	double		m_dAvgElapsedTime;

	__int64		m_nPrevTime;
	__int64		m_nCurrTime;

	WCHAR		m_wcName[50];
	WCHAR		m_wcTimeInfo[50];

public:
	CStopWatch(const WCHAR* pwName);
	virtual ~CStopWatch() {}

	void BeginTimer();
	float EndTimer(float fElapsedTime);

	float GetElapsedTime() {
		return static_cast<float>(m_dDeltaTime);
	}
	float GetAvgElapsedTime() {
		return (float)m_dAvgElapsedTime;
	}
	WCHAR* GetText() {
		return m_wcTimeInfo;
	}
};

#define StopWatchMananger CStopWatchManager::GetInstance()
class CStopWatchManager {
private:
	static CStopWatchManager *m_pInstance;
	std::vector<CStopWatch*> m_vpStopWatch;

private:
	CStopWatchManager() {}
	~CStopWatchManager() {}

public:
	static void CreateInstance() {
		if (m_pInstance) return;
		m_pInstance = new CStopWatchManager;
	}
	static CStopWatchManager* GetInstance() {
		return m_pInstance;
	}

	void CreateUI(const POINT& vPoint);

	void AddStopWatch(const WCHAR* pwsName) {
		m_vpStopWatch.push_back(new CStopWatch(pwsName));
	}
	void BeginTimer(UINT idx) {
		m_vpStopWatch[idx]->BeginTimer();
	}
	float EndTimer(UINT idx, float fElapsedTime) {
		return m_vpStopWatch[idx]->EndTimer(fElapsedTime);
	}
};
_declspec(selectany) CStopWatchManager *CStopWatchManager::m_pInstance = nullptr;