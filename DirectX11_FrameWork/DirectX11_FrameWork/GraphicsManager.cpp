#include "stdafx.h"
#include "GraphicsManager.h"


GraphicsManager::GraphicsManager()
{
}


GraphicsManager::~GraphicsManager()
{
}

bool GraphicsManager::Initialize(int, int, HWND)
{
	return true;
}

void GraphicsManager::Shutdown()
{
}

bool GraphicsManager::Frame()
{
	return true;
}

bool GraphicsManager::Render()
{
	return true;
}
