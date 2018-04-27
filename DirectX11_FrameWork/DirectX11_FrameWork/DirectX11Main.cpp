
#include "stdafx.h"
#include "SystemApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{

	SystemApp MainApp;

	if (MainApp.Initialize())
	{
		MainApp.Run();
	}

	MainApp.Shutdown();

	return 0;
}