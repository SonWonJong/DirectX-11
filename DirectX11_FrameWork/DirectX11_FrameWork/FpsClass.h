#pragma once

#pragma comment(lib, "winmm.lib")

class FpsClass
{
public:
	FpsClass();
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_count;
	unsigned long m_startTime;
};

