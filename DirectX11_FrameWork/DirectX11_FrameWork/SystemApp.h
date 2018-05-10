#pragma once

class SystemApp
{
public:
	SystemApp();
	SystemApp(const SystemApp&);
	~SystemApp();

	bool Initialize();
	void Shutdown();
	void Run();
	
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_ApplicationName;
	HINSTANCE m_Hinstance;
	HWND m_Hwnd;

	class GraphicsManager* m_Graphics = nullptr;
	class FpsClass*	m_Fps = nullptr;
	class CpuClass* m_Cpu = nullptr;
	class TimerClass* m_Timer = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemApp* ApplicationHandle = 0;