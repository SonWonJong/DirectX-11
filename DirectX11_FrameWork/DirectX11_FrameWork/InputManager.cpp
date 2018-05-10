#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

bool InputManager::Initialize(HINSTANCE InHinstance, HWND InHwnd, int InScreenWidth, int InScreenHeight)
{
	m_ScreenWidth = InScreenWidth;
	m_ScreenHeight = InScreenHeight;
	m_MouseX = m_ScreenWidth / 2;
	m_MouseY = m_ScreenHeight / 2;

	// Direct Input �������̽��� �ʱ�ȭ �մϴ�.
	HRESULT result = DirectInput8Create(InHinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_Keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}
	result = m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}
	// �ٸ� ���α׷��� �������� �ʵ��� Ű������ ���� ������ ����
	result = m_Keyboard->SetCooperativeLevel(InHwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Ű���带 �Ҵ�
	result = m_Keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// ���콺 Direct Input �������̽��� ����
	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� ������ ������ ����
	result = m_Mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� ���� ������ ����
	result = m_Mouse->SetCooperativeLevel(InHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� �Ҵ�
	result = m_Mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputManager::Shutdown()
{
	// ���콺�� ��ȯ�մϴ�.
	if (m_Mouse)
	{
		m_Mouse->Unacquire();
		m_Mouse->Release();
		m_Mouse = nullptr;
	}

	// Ű���带 ��ȯ�մϴ�.
	if (m_Keyboard)
	{
		m_Keyboard->Unacquire();
		m_Keyboard->Release();
		m_Keyboard = nullptr;
	}

	// m_directInput ��ü�� ��ȯ�մϴ�.
	if (m_DirectInput)
	{
		m_DirectInput->Release();
		m_DirectInput = nullptr;
	}
}

bool InputManager::Frame()
{
	if (!ReadKeyboard())
	{
		return false;
	}

	if (!ReadMouse())
	{
		return false;
	}

	ProcessInput();

	return true;
}

bool InputManager::ReadKeyboard()
{
	// Ű���� ����̽��� ��´�.
	HRESULT result = m_Keyboard->GetDeviceState(sizeof(m_KeyboardState), (LPVOID)&m_KeyboardState);
	if (FAILED(result))
	{
		// Ű���尡 ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� ��Ʈ���� �ٽ� ���� �´�
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputManager::ReadMouse()
{
	// ���콺 ����̽��� ��´�.
	HRESULT result = m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
	if (FAILED(result))
	{
		// ���콺�� ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� ��Ʈ���� �ٽ� �����´�
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputManager::ProcessInput()
{
	// ������ ���� ���콺 ��ġ�� ������ ������� ���콺 Ŀ���� ��ġ�� ������Ʈ �մϴ�.
	m_MouseX += m_MouseState.lX;
	m_MouseY += m_MouseState.lY;

	if (m_MouseState.lX > 1)
	{
		int j = 0;
		j = 11;
	}

	// ���콺 ��ġ�� ȭ�� �ʺ� �Ǵ� ���̸� �ʰ����� �ʴ��� Ȯ���Ѵ�.
	if (m_MouseX < 0) 
		m_MouseX = 0;
	if (m_MouseY < 0) 
		m_MouseY = 0; 
	if (m_MouseX > m_ScreenWidth) 
		m_MouseX = m_ScreenWidth;
	if (m_MouseY > m_ScreenHeight) 
		m_MouseY = m_ScreenHeight;
}

void InputManager::GetMouseLocation(int& InMouseX, int& InMouseY)
{
	InMouseX = m_MouseX;
	InMouseY = m_MouseY;
}

bool InputManager::IsKeyPressed(int InCheckKey)
{
	if (InCheckKey < MaxKeyCount && m_KeyboardState[InCheckKey] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::IsEscapePressed()
{
	if (m_KeyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::IsLeftPressed()
{
	if (m_KeyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsRightPressed()
{
	if (m_KeyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsUpPressed()
{
	if (m_KeyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsDownPressed()
{
	if (m_KeyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsAPressed()
{
	if (m_KeyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsZPressed()
{
	if (m_KeyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsPgUpPressed()
{
	if (m_KeyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsPgDownPressed()
{
	if (m_KeyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}
