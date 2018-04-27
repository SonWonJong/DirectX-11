#include "stdafx.h"
#include "InputManager.h"


InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void InputManager::Initialize()
{
	for (int i = 0; i<256; ++i)
	{
		m_Keys[i] = false;
	}
}

void InputManager::KeyDown(unsigned int Input)
{
	m_Keys[Input] = true;
}

void InputManager::KeyUp(unsigned int Input)
{
	m_Keys[Input] = true;
}

bool InputManager::IsKeyDown(unsigned int Input)
{
	return m_Keys[Input];
}
